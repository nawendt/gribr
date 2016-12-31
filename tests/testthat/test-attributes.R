context("GRIB file attributes")

test_that("GRIB file attributes are correct", {
  g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
  mmg <- grib_open(system.file("extdata", "multi_created.grib1", package = "gribr"))

  # non-GRIB file access
  expect_error(grib_open(system.file("extdata", "dummy.grib", package = "gribr")), "not GRIB")

  # length
  expect_equal(grib_length(g), 64)

  # length (with multi-message support)
  expect_equal(grib_length(mmg), 21)

  # class
  expect_s3_class(g, "grib")

  # list contents
  ## data.frame output
  contents <- grib_list(g)
  expect_equal(dim(contents)[1], 64)
  expect_s3_class(contents, "data.frame")

  ## char output
  contents <- grib_list(g, output = "string")
  expect_equal(length(contents), 64)
  expect_type(contents, "character")

  # closing
  capture_output(grib_close(g))
  capture_output(grib_close(mmg))
  expect_equal(is.null.externalptr(g$handle), TRUE)

  # access after closing
  expect_error(grib_length(g), "closed")
  expect_error(grib_get_message(g, 1), "closed")
  expect_error(grib_select(g, list(shortName = "2t")), "closed")
})


