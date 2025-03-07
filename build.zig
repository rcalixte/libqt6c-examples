const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const enable_workaround = b.option(bool, "enable-workaround", "Enable workaround for missing Qt C++ headers") orelse false;
    const skip_restricted = b.option(bool, "skip-restricted", "Skip restricted libraries") orelse false;

    const is_bsd_family = switch (target.result.os.tag) {
        .dragonfly, .freebsd, .netbsd, .openbsd => true,
        else => false,
    };

    var arena = std.heap.ArenaAllocator.init(b.allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    // Find all main.c files
    var main_files: std.ArrayListUnmanaged(struct { dir: []const u8, path: []const u8 }) = .empty;

    {
        var dir = try std.fs.cwd().openDir(".", .{ .iterate = true });
        var walker = try dir.walk(b.allocator);
        defer walker.deinit();

        while (try walker.next()) |entry| {
            if (entry.kind == .file and std.mem.eql(u8, entry.basename, "main.c")) {
                const parent_dir = std.fs.path.dirname(entry.path) orelse continue;
                try main_files.append(allocator, .{
                    .dir = try b.allocator.dupe(u8, parent_dir),
                    .path = try b.allocator.dupe(u8, entry.path),
                });
            }
        }
    }

    if (main_files.items.len == 0) {
        @panic("No main.c files found.\n");
    }

    // Qt system libraries to link
    const qt_libs = &[_][]const u8{
        "Qt6Core",
        "Qt6Gui",
        "Qt6Widgets",
        "Qt6Multimedia",
        "Qt6MultimediaWidgets",
        "Qt6PrintSupport",
        "Qt6SvgWidgets",
        "Qt6WebEngineCore",
        "Qt6WebEngineWidgets",
    };

    const qt6c = b.dependency("libqt6c", .{
        .target = target,
        .optimize = .ReleaseFast,
        .@"enable-workaround" = enable_workaround or is_bsd_family,
        .@"skip-restricted" = skip_restricted,
    });

    const c_flags = &.{
        "-O2",
    };

    // Create an executable for each main.c
    for (main_files.items) |main| {
        const exe_name = std.fs.path.basename(main.dir);
        const exe = b.addExecutable(.{
            .name = exe_name,
            .target = target,
            .optimize = optimize,
        });

        // Add include path
        exe.root_module.addIncludePath(qt6c.path("include"));

        // Link Qt system libraries
        if (is_bsd_family) {
            exe.root_module.addLibraryPath(std.Build.LazyPath{ .cwd_relative = "/usr/local/lib/qt6" });
        }

        for (qt_libs) |lib| {
            exe.root_module.linkSystemLibrary(lib, .{});
        }

        // Link libqt6c static libraries
        for (qt6c.builder.install_tls.step.dependencies.items) |qt_lib| {
            if (std.mem.indexOf(u8, qt_lib.name, " ")) |index| {
                const qt_lib_name = qt_lib.name[index + 1 ..];
                exe.root_module.linkLibrary(qt6c.artifact(qt_lib_name));
            }
        }

        // Add main build source file
        exe.root_module.addCSourceFile(.{
            .file = b.path(main.path),
            .flags = c_flags,
        });

        // Install the executable
        b.installArtifact(exe);

        // Create a run step
        const exe_install = b.addInstallArtifact(exe, .{});
        const run_cmd = b.addRunArtifact(exe);
        run_cmd.step.dependOn(&exe_install.step);

        const run_description = try std.fmt.allocPrint(allocator, "Run the {s} example", .{exe_name});

        const run_step = b.step(exe_name, run_description);
        run_step.dependOn(&run_cmd.step);
    }
}
