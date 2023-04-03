context("GRIB message retrieval and attributes")

test_that("Incorrect message requests are handled correctly", {
  g <- grib_open(system.file("extdata", "lfpw.grib1", package = "gribr"))

#   grib_get_message
  expect_error(grib_get_message(g, -1), "out of bounds")
  expect_error(grib_get_message(g, 0), "out of bounds")
  expect_error(grib_get_message(g, 999), "out of bounds")
  gm <- grib_get_message(g, 1)
  expect_s3_class(gm, "gribMessage")
  gm <- grib_get_message(g, c(1, 2))
  expect_equal(length(gm), 2)
  expect_equal(all(sapply(gm, class) == "gribMessage"), TRUE)

#   grib_select
  expect_error(grib_select(g, list(dne = "dne")), "not found")
  expect_error(grib_select(g, list(shortName = "dne")), "no messages matched")
  expect_error(grib_select(g, list()), "keyPairs input")
  expect_error(grib_select(g, c(dne = "dne")), "must be a list")
  gm <- grib_select(g, list(shortName = c("2t", "2d")))
  expect_equal(length(gm), 2)
  expect_equal(all(sapply(gm, class) == "gribMessage"), TRUE)

#   grib_cube
  expect_error(grib_cube(g, "dne", "isobaricInhPa"), "no messages matched")
  expect_error(grib_cube(g, "u", "dne"), "no messages matched")
  cube <- grib_cube(g, "u", "isobaricInhPa", TRUE)
  expect_is(cube, "array")
  expect_equal(length(dim(cube)), 3)

#   keys
  expect_equal(has.key(gm[[1]], "shortName"), TRUE)
  expect_equal(has.key(gm[[1]], "dne"), FALSE)

  bad <- grib_open(system.file("extdata", "bad_key.grib", package = "gribr"))
  expect_warning(grib_get_message(bad, 1), "gribr: unable to get")

  capture_output(grib_close(g))
})
