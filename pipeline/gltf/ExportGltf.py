import bpy
import sys
import os
import argparse


def parse_arguments():
    """
    Parse arguments supplied after Blender's '--' separator.

    Example:
        blender my_level.blend --background --python ExportGltf.py --
            output/my_level.gltf --animationSampleStep 4
    """

    argv = sys.argv

    if "--" in argv:
        argv = argv[argv.index("--") + 1:]
    else:
        argv = []

    parser = argparse.ArgumentParser(
        description="Export the current Blender scene as a separate GLTF file."
    )

    parser.add_argument(
        "output",
        help="Output GLTF file path."
    )

    parser.add_argument(
        "--animationSampleStep",
        type=int,
        default=1,
        help=(
            "Number of frames between animation samples. "
            "For example, 4 samples every 4th frame. Default: 1."
        )
    )

    args = parser.parse_args(argv)

    if args.animationSampleStep < 1:
        parser.error("animationSampleStep must be >= 1")

    return args


def export_gltf(output_path, animation_sample_step):
    # Make the output path absolute.
    output_path = os.path.abspath(output_path)

    # Create the output directory if it doesn't already exist.
    output_directory = os.path.dirname(output_path)

    if output_directory:
        os.makedirs(output_directory, exist_ok=True)

    print("Exporting GLTF:")
    print(f"  Output: {output_path}")
    print(f"  Animation sample step: {animation_sample_step} frame(s)")

    bpy.ops.export_scene.gltf(
        filepath=output_path,
        export_format='GLTF_SEPARATE',
        export_extras=True,
        export_animations=True,
        export_frame_step=animation_sample_step,
        export_yup=True,
        use_selection=False,
    )

    print("GLTF export complete.")


def main():
    args = parse_arguments()

    export_gltf(
        args.output,
        args.animationSampleStep
    )


if __name__ == "__main__":
    main()
