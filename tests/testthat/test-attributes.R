context("GRIB file attributes")

test_that("GRIB file attributes are correct", {
  g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))
  mmg <- grib_open(system.file("extdata", "multi_created.grib1", package = "gribr"))

  # length
  expect_equal(grib_length(g), 64)

  # length (with multi-message support)
  expect_equal(grib_length(mmg), 21)

})


