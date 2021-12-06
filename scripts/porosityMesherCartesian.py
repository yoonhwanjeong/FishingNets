import argparse
import math
import textwrap


class Vertex:
    def __init__(self, label, x, y, z):
        self.label = label
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f"({self.x} {self.y} {self.z}) // point {self.label}"


class Block:
    def __init__(self, vertices, resolution, name):
        self.vertices = vertices
        self.resolution = resolution
        self.name = name

    def __getitem__(self, item):
        return list(map(lambda x: self.vertices[x], item))

    def __str__(self):
        return f"""\
hex ({" ".join(map(str, self.vertices))})
{self.name} ({" ".join(map(str, self.resolution))}) simpleGrading (1 1 1)"""


class Face:
    def __init__(self, vertices):
        self.vertices = vertices

    def __str__(self):
        return f"""({" ".join(map(str, self.vertices))})"""


class Patch:
    def __init__(self, faces, type, name):
        self.faces = faces
        self.type = type
        self.name = name

    def __str__(self):
        n = "\n"
        return f"""\
{self.type} {self.name}
(
{n.join(map(lambda x: textwrap.indent(str(x), "    "), self.faces))}
)"""


class BlockMesh:
    def __init__(self, dimension, panel, angle, inflation, scale, resolution):
        self.center = list(map(lambda x: x / 2, dimension))
        self.dx, self.dy, self.dz = [x / y for x, y in zip(dimension, resolution)]
        self.panel = panel
        self.angle = angle
        self.scale = scale
        self.resolution = resolution
        self.domain_vertices = []
        self.domain_blocks = []
        self.patches = []
        self.create_blocks()
        self.create_patches()

    def create_blocks(self):
        index = 0
        for i in range(self.resolution[0] + 1):
            for j in range(self.resolution[1] + 1):
                for k in range(self.resolution[2] + 1):
                    self.domain_vertices.append(Vertex(index, i * self.dx, j * self.dy, k * self.dz))
                    index += 1
        index = 0
        di = (self.resolution[1] + 1) * (self.resolution[2] + 1)
        dj = self.resolution[2] + 1
        for i in range(self.resolution[0]):
            for j in range(self.resolution[1]):
                for k in range(self.resolution[2]):
                    self.domain_blocks.append(Block([index, index + di, index + di + dj, index + dj, index + 1, index + di + 1, index + di + dj + 1, index + dj + 1], [2, 2, 2], f"Block{index}"))
                    index += 1

    def create_patches(self):
        panel_samples = max(self.resolution[1:3]) * 2
        pdx, pdy, pdz = list(map(lambda x: x / panel_samples, self.panel))
        for i in range(panel_samples):
            for j in range(panel_samples):
                for k in range(panel_samples):
                    x, y, z = [x - y / 2 for x, y in zip([x * y for x, y in zip([i, j, k], [pdx, pdy, pdz])], self.panel)]
                    x, z = x * math.cos(self.angle) + z * math.sin(self.angle), -x * math.sin(self.angle) + z * math.cos(self.angle)
                    index_x, index_y, index_z = [x // y for x, y in zip([x + y for x, y in zip([x, y, z], self.center)], [self.dx, self.dy, self.dz])]
                    index = int(index_x * (self.resolution[1] + 1) * (self.resolution[2] + 1) + index_y * (self.resolution[2] + 1) + index_z)
                    self.domain_blocks[index].name = "panel"
        index = 0
        inlet_faces = []
        outlet_faces = []
        side_faces = []
        for i in range(self.resolution[0]):
            for j in range(self.resolution[1]):
                for k in range(self.resolution[2]):
                    if i == 0:
                        inlet_faces.append(Face(self.domain_blocks[index][[0, 3, 4, 7]]))
                    elif i == self.resolution[0] - 1:
                        outlet_faces.append(Face(self.domain_blocks[index][[1, 2, 5, 6]]))
                    if j == 0:
                        side_faces.append(Face(self.domain_blocks[index][[0, 1, 4, 5]]))
                    elif j == self.resolution[1] - 1:
                        side_faces.append(Face(self.domain_blocks[index][[2, 3, 6, 7]]))
                    if k == 0:
                        side_faces.append(Face(self.domain_blocks[index][[0, 1, 2, 3]]))
                    elif k == self.resolution[2] - 1:
                        side_faces.append(Face(self.domain_blocks[index][[4, 5, 6, 7]]))
                    index += 1
        self.patches.append(Patch(inlet_faces, "patch", "inlet"))
        self.patches.append(Patch(outlet_faces, "patch", "outlet"))
        self.patches.append(Patch(side_faces, "wall", "sideWalls"))

    def __str__(self):
        n = "\n"
        s = f"""\
FoamFile
{{
    version 2.0;
    format  ascii;
    class   dictionary;
    object  blockMeshDict;
}}

scale   {self.scale};

vertices
(
{n.join(map(lambda x: textwrap.indent(str(x), "    "), self.domain_vertices))}
);

blocks
(
{(n + n).join(map(lambda x: textwrap.indent(str(x), "    "), self.domain_blocks))}
);

edges
(
);

patches
(
{(n + n).join(map(lambda x: textwrap.indent(str(x), "    "), self.patches))}
);

mergePatchPairs
(
);
"""
        return s


if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("--dimension", nargs=3, default=[2, 2, 1], type=float, metavar=("length", "width", "height"))
    parser.add_argument("--panel", nargs=3, default=[0.4, 0.4, 0.1], type=float, metavar=("length", "width", "thickness"))
    parser.add_argument("--angle", default=30, type=float, metavar="degrees")
    parser.add_argument("--inflation", default=20, type=float, metavar="inflation")
    parser.add_argument("--scale", default=1, type=float, metavar="scale")
    parser.add_argument("--resolution", nargs=3, default=[40, 40, 20], type=int, metavar=("x", "y", "z"))
    parser.add_argument("-o", "--output", default="blockMeshDict", type=str, metavar="output")
    args = parser.parse_args()
    args.angle = math.radians(args.angle)
    print(f"""\
dimension:  {args.dimension}
panel:      {args.panel}
angle:      {args.angle}
inflation:  {args.inflation}
scale:      {args.scale}
resolution: {args.resolution}
output:     {args.output}""")
    block_mesh = BlockMesh(args.dimension, args.panel, args.angle, args.inflation, args.scale, args.resolution)
    with open(args.output, "w") as file:
        file.write(str(block_mesh))
