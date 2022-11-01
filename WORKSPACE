workspace(name = "cache")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_jar")

http_archive(
    name = "bazel_compdb",
    #patches = ["//foreign:comp_db.patch"],
    sha256 = "d51f8168954d4aa0ca984f53a1a6be298d827ff39303d10522dffb2a5c1942dc",
    strip_prefix = "bazel-compilation-database-0.5.0",
    urls = ["https://github.com/grailbio/bazel-compilation-database/archive/0.5.0.tar.gz"],
)
