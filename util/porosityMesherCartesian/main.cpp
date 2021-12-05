#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

std::ofstream outfile ("blockMeshDict");

class Vertex
{
public:
    double x, y, z;
    int label;

    Vertex()
    {
        x = 0;
        y = 0;
        z = 0;
        label = 0;
    }
    Vertex(int label, double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->label = label;
    }

    void print() const
    {
        outfile << "\t( "<<x<<" "<<y<<" "<<z<<" ) // point "<< label<<"\n";
    }

    Vertex& rotateX(double angle, const Vertex& other)
    {
        x = x - other.x;
        y = y - other.y;
        z = z - other.z;
        double new_x = x;
        double new_y = y * cos(angle) - z * sin(angle);
        double new_z = y * sin(angle) + z * cos(angle);
        x = other.x + new_x;
        y = other.y + new_y;
        z = other.z + new_z;
        return *this;
    }

    Vertex& rotateY(double angle, const Vertex& other)
    {
        x = x - other.x;
        y = y - other.y;
        z = z - other.z;
        double new_x = x * cos(angle) + z * sin(angle);
        double new_y = y;
        double new_z = -x * sin(angle) + z * cos(angle);
        x = other.x + new_x;
        y = other.y + new_y;
        z = other.z + new_z;
        return *this;
    }

    Vertex& rotateZ(double angle, const Vertex& other)
    {
        x = x - other.x;
        y = y - other.y;
        z = z - other.z;
        double new_x = x * cos(angle) - y * sin(angle);
        double new_y = x * sin(angle) + y * cos(angle);
        double new_z = z;
        x = other.x + new_x;
        y = other.y + new_y;
        z = other.z + new_z;
        return *this;
    }

    Vertex& rotate(double angleX, double angleY, double angleZ, const Vertex& other)
    {
        this->rotateX(angleX, other).rotateY(angleY, other).rotateZ(angleZ, other);
        return *this;
    }

    Vertex& operator+=(const Vertex& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

};

class Block
{
public:

    int vertices[9];
    std::string name;
    int resolution[3];

    Block()
    {
        for(int i = 0; i < 8; i ++)
        {
            vertices[i] = 0;
        }
        name = "";
        resolution[0] = 1;
        resolution[1] = 1;
        resolution[2] = 1;
    }
    Block(const int* vertices, const int* resolution, const std::string& name)
    {
        for(int i = 0; i < 8; i ++)
        {
            this->vertices[i] = vertices[i];
        }
        this->name = name;
        this->resolution[0] = resolution[0];
        this->resolution[1] = resolution[1];
        this->resolution[2] = resolution[2];
    }

    void print()
    {
        outfile <<"\thex ( ";
        for(int i = 0; i < 8; i ++)
            outfile<<vertices[i]<<" ";
        outfile<<")\n\t"<<name<<" ( "<<resolution[0]<<" "<<resolution[1]<<" "<<resolution[2]<<" ) simpleGrading (1 1 1)\n";
    }

};

class Face
{
public:
    int vertices[5]{};

    Face()
    {
        for(int i = 0; i < 4; i ++)
            vertices[i] = 0;
    }

    explicit Face(const int* vertices)
    {
        for(int i = 0; i < 4; i ++)
        {
            this->vertices[i] = vertices[i];
        }
    }

    void print()
    {
        outfile<<"\t( ";
        for(int i = 0; i < 4; i ++)
            outfile<<vertices[i]<<" ";
        outfile<<")\n";
    }
};

class Patch
{
public:
    Face* faces;
    std::string type, name;
    int numFaces;

    Patch()
    {
        type = "";
        name = "";
        faces = nullptr;
        numFaces = 0;
    }
    Patch(Face* faces, int len, const std::string& type, const std::string& name)
    {
        this->type = type;
        this->name = name;

        this->faces = new Face[len+1];
        for(int i = 0; i < len; i ++)
            this->faces[i] = faces[i];
        numFaces = len;

    }

    void print() const
    {
        outfile<<"\t"<<type<<" "<<name<<"\n";
        outfile<<"\t"<<"(\n";
        for(int i = 0; i < numFaces; i ++)
        {
            outfile<<"\t";
            faces[i].print();
        }
        outfile<<"\t"<<")\n";
    }
};

class Panel
{
public:

};

class blockMesh
{
public:
    Vertex* domainVertices;
    Block* domainBlocks;
    Patch patches[3];
    Face *inletFaces;
    Face *outletFaces;
    Face *sideFaces;
    double scale;
    double center[3];
    double angle;
    double dx, dy, dz;
    double panel[3];
    int resolution[3];

    blockMesh()
    {
        domainBlocks = nullptr;
        domainVertices = nullptr;
        for(auto & patch : patches)
            patch = Patch();
        scale = 1;
        resolution[0] = 1;
        resolution[1] = 1;
        resolution[2] = 1;
        center[0] = 0;
        center[1] = 0;
        center[2] = 0;
        dx = 0.1;
        dy = 0.1;
        dz = 0.1;
        angle = 0;
        panel[0] = 0;
        panel[1] = 0;
        panel[2] = 0;
        inletFaces = nullptr;
        outletFaces = nullptr;
        sideFaces = nullptr;

    }

    blockMesh(double dLength, double dWidth, double dHeight, double pLength, double pWidth, double pThickness, double AoA, double inflation, double scale, int* resolution)
    {
        this->scale = scale;
        this->resolution[0] = resolution[0];
        this->resolution[1] = resolution[1];
        this->resolution[2] = resolution[2];
        this->center[0] = dLength / 2;
        this->center[1] = dWidth / 2;
        this->center[2] = dHeight / 2;
        dx = dLength / resolution[0];
        dy = dWidth / resolution[1];
        dz = dHeight / resolution[2];
        angle = AoA;
        panel[0] = pLength;
        panel[1] = pWidth;
        panel[2] = pThickness;
        inletFaces = new Face[resolution[1] * resolution[2]];
        outletFaces = new Face[resolution[1] * resolution[2]];
        sideFaces = new Face[resolution[0] * resolution[2] * 4];
        domainBlocks = new Block[(resolution[0]) * (resolution[1]) * (resolution[2])];
        createBlocks();
        createPatches();
    }

    void print()
    {
        outfile<<"FoamFile\n{\n";
        outfile<<"\tversion\t2.0;\n";
        outfile<<"\tformat\tascii;\n";
        outfile<<"\tclass\tdictionary;\n";
        outfile<<"\tobject\tblockMeshDict;\n";
        outfile<<"}\n\n";
        outfile<<"scale\t"<<scale<<";\n\n";
        printVertices();
        outfile<<"\n";
        printBlocks();
        outfile<<"\nedges\n(\n);\n";
        outfile<<"patches\n(\n";
        printPatches();
        outfile<<");\n\n";
        outfile<<"mergePatchPairs\n(\n);\n";
    }
    void printVertices()
    {
        outfile<<"vertices\n(\n";
        for(int i = 0; i < (resolution[0] + 1) * (resolution[1] + 1) * (resolution[2] + 1); i ++)
        {
            domainVertices[i].print();
        }
        outfile<<");\n";
    }

    void printBlocks()
    {
        outfile<<"blocks\n(\n";
        for(int i = 0; i < (resolution[0]) * (resolution[1]) * (resolution[2]); i ++)
        {
            domainBlocks[i].print();
            outfile<<"\n";
        }
        outfile<<");\n";
    }

    blockMesh& createBlocks()
    {
        int index = 0;
        domainVertices = new Vertex[(resolution[0]+1) * (resolution[1]+1) * (resolution[2]+1)];
        for(auto i = 0; i < resolution[0] + 1; i ++)
        {
            for(auto j = 0; j < resolution[1] + 1; j ++)
            {
                for(auto k = 0; k < resolution[2] + 1; k ++)
                {
                    index = i * (resolution[1] + 1) * (resolution[2] + 1) + j * (resolution[2] + 1) + k;
                    domainVertices[index] = Vertex(index, i * dx, j * dy, k * dz);
                }
            }
        }

        int ind = 0;
        for(auto i = 0; i < resolution[0]; i ++)
        {
            for(auto j = 0; j < resolution[1]; j ++)
            {
                for(auto k = 0; k < resolution[2]; k ++)
                {
                    index = i * (resolution[1]+1) * (resolution[2]+1) + j * (resolution[2]+1) + k;
                    int b1 = index;
                    int b2 = (i+1) * (resolution[1]+1) * (resolution[2]+1) + j * (resolution[2]+1) + k;
                    int b3 = (i+1) * (resolution[1]+1) * (resolution[2]+1) + (j+1) * (resolution[2]+1) + k;
                    int b4 = (i) * (resolution[1]+1) * (resolution[2]+1) + (j + 1) * (resolution[2]+1) + k;
                    int b5 = i * (resolution[1]+1) * (resolution[2]+1) + j * (resolution[2]+1) + k + 1;
                    int b6 = (i+1) * (resolution[1]+1) * (resolution[2]+1) + j * (resolution[2]+1) + k + 1;
                    int b7 = (i+1) * (resolution[1]+1) * (resolution[2]+1) + (j+1) * (resolution[2]+1) + k + 1;
                    int b8 = (i) * (resolution[1]+1) * (resolution[2]+1) + (j + 1) * (resolution[2]+1) + k + 1;
                    int Vertices[] = {b1, b2, b3, b4, b5, b6, b7, b8};
                    int res[] = {2, 2, 2};
                    domainBlocks[ind++] = Block(Vertices, res, "Block" + std::to_string(index));
                }
            }
        }

        return *this;
    }

    void printPatches()
    {
        for(const auto & patch : patches)
        {
            patch.print();
            outfile<<"\n";
        }
    }

    blockMesh& createPatches()
    {
        int panelSamples = std::max(resolution[1], resolution[2]) * 2;
        double pdx = panel[0] / panelSamples;
        double pdy = panel[1] / panelSamples;
        double pdz = panel[2] / panelSamples;
        for(auto i = 0; i < (panelSamples); i ++)
        {
            for(auto j = 0; j < panelSamples; j ++)
            {
                for(auto k = 0; k < panelSamples; k ++)
                {
                    Vertex panelVertex(1, i * pdx - panel[0] / 2, j * pdy - panel[1] / 2, k * pdz - panel[2] / 2);
                    panelVertex.rotateY(angle, Vertex(1, 0, 0, 0));
                    double panelPoint[] = {panelVertex.x + center[0], panelVertex.y + center[1], panelVertex.z + center[2]};
                    int indexX = (int)(panelPoint[0] / dx);
                    int indexY = (int)(panelPoint[1] / dy);
                    int indexZ = (int)(panelPoint[2] / dz);
                    int index = (resolution[1] + 1)*(resolution[2]+1) * indexX + (resolution[2] + 1) * indexY + indexZ;
                    domainBlocks[index].name = "panel";
                }
            }
        }

        int indInlet = 0;
        int indOutlet = 0;
        int indSide = 0;
        int vert[4];


        for(auto i = 0; i < resolution[0]; i ++)
        {
            for(auto j = 0; j < resolution[1]; j ++)
            {
                for(auto k = 0; k < resolution[2]; k ++)
                {
                    int index = i * (resolution[1]) * (resolution[2]) + j * (resolution[2]) + k;
                    if( i == 0)
                    {
                        vert[0] = domainBlocks[index].vertices[0];
                        vert[1] = domainBlocks[index].vertices[3];
                        vert[2] = domainBlocks[index].vertices[4];
                        vert[3] = domainBlocks[index].vertices[7];
                        Face f(vert);
                        inletFaces[indInlet++] = f;
                    }
                    if(i == resolution[0] - 1)
                    {
                        vert[0] = domainBlocks[index].vertices[1];
                        vert[1] = domainBlocks[index].vertices[2];
                        vert[2] = domainBlocks[index].vertices[5];
                        vert[3] = domainBlocks[index].vertices[6];
                        Face f(vert);
                        outletFaces[indOutlet++] = f;
                    }
                    if(j == 0)
                    {
                        vert[0] = domainBlocks[index].vertices[0];
                        vert[1] = domainBlocks[index].vertices[1];
                        vert[2] = domainBlocks[index].vertices[4];
                        vert[3] = domainBlocks[index].vertices[5];
                        Face f(vert);
                        sideFaces[indSide++] = f;
                    }
                    if(j == resolution[1] - 1)
                    {
                        vert[0] = domainBlocks[index].vertices[2];
                        vert[1] = domainBlocks[index].vertices[3];
                        vert[2] = domainBlocks[index].vertices[6];
                        vert[3] = domainBlocks[index].vertices[7];
                        Face f(vert);
                        sideFaces[indSide++] = f;
                    }
                    if(k == 0)
                    {
                        vert[0] = domainBlocks[index].vertices[0];
                        vert[1] = domainBlocks[index].vertices[1];
                        vert[2] = domainBlocks[index].vertices[2];
                        vert[3] = domainBlocks[index].vertices[3];
                        Face f(vert);
                        sideFaces[indSide++] = f;
                    }
                    if(k == resolution[2] - 1)
                    {
                        vert[0] = domainBlocks[index].vertices[4];
                        vert[1] = domainBlocks[index].vertices[5];
                        vert[2] = domainBlocks[index].vertices[6];
                        vert[3] = domainBlocks[index].vertices[7];
                        Face f(vert);
                        sideFaces[indSide++] = f;
                    }
                }
            }
        }

        Patch inlet = Patch(inletFaces, indInlet, "patch", "inlet");
        Patch outlet = Patch(outletFaces, indOutlet, "patch", "outlet");
        Patch sideWalls = Patch(sideFaces, indSide, "wall", "sideWalls");
        patches[0] = inlet;
        patches[1] = outlet;
        patches[2] = sideWalls;

        return *this;
    }

};

int main() {
    double dLength = 2, dWidth = 2, dHeight = 1;
    double pLength = 0.4, pWidth = 0.4, pThickness = 0.1;
    double AoA = acos(-1) / 6;
    double inflation = 20;
    double scale = 1.0;
    int resolution[3] = {40, 40, 20};

    blockMesh B(dLength, dWidth, dHeight, pLength, pWidth, pThickness, AoA, inflation, scale, resolution);
    B.print();

    return 0;
}
