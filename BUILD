package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cache",
    hdrs = ["cache.hh"],
    srcs = ["cache.cc"],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//:cache"],
)
