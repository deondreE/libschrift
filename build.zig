const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{ .name = "libschrift", .root_source_file = b.path("src/root.zig"), .target = target, .optimize = optimize, .link_libc = true });
    lib.addIncludePath(b.path("./util/"));
    lib.addCSourceFiles(.{ .files = &.{ "schrift.c", "schrift.h" } });

    b.installArtifact(lib);
}
