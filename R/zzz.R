.gribr_env <- new.env(parent = emptyenv())

.onLoad <- function(libname, pkgname) {
  orig_val <- Sys.getenv("ECCODES_DEFINITION_PATH")
  assign("orig_eccodes_path", orig_val, envir = .gribr_env)

  if (.Platform$OS.type == "windows" && orig_val == "") {
    env_names <- names(Sys.getenv())
    rtools_ev <- env_names[grep("^R_RTOOLS[0-9]+_PATH$", env_names)]
    
    if (length(rtools_ev) > 0) {
      full_path_str <- Sys.getenv(rtools_ev[1])
      first_path <- strsplit(full_path_str, ";")[[1]][1]
      rtools_root <- sub("/[^/]+/[^/]+$", "", first_path)
      
      if (rtools_root != "" && dir.exists(rtools_root)) {
        def_path <- file.path(rtools_root, "ucrt64", "share", "eccodes", "definitions")
        if (dir.exists(def_path)) {
          Sys.setenv(ECCODES_DEFINITION_PATH = normalizePath(def_path, winslash = "/"))
        }
      }
    }
  }
}

.onUnload <- function(libpath) {
  orig_val <- get0("orig_eccodes_path", envir = .gribr_env, ifnotfound = "")
  
  if (orig_val == "") {
    Sys.unsetenv("ECCODES_DEFINITION_PATH")
  } else {
    Sys.setenv(ECCODES_DEFINITION_PATH = orig_val)
  }
}
