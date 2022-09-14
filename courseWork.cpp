#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int *readingV(int &, int *);
int **readingU(int &, int **);
int *readingUWeights(int &, int *);
std::vector<int> stronglyConnectedComponent(int &, int *, int **);
std::vector<int> VFirst(int &, int **);
std::vector<int> VLast(int &, int **);
size_t countNumOfV();
size_t countNumOfU();
void reallocUp2D(int **, int, int);
void reallocUp1D(int *, int, int);

int main()
{
    setlocale(LC_ALL, "ukr");

    int numOfV = countNumOfV();
    int numOfU = countNumOfU();
    int *V = new int[numOfV];
    int **U = new int *[numOfU];
    int *UWeights = new int[numOfU];
    for (size_t i = 0; i < numOfU; i++)
    {
        U[i] = new int[numOfU];
    }

    auto inputV = readingV(numOfV, V);
    auto inputU = readingU(numOfU, U);
    auto inputUWeights = readingUWeights(numOfU, UWeights);

    int *VToCheck = new int[numOfV];
    for (size_t i = 0; i < numOfV; i++)
    {
        VToCheck[i] = inputV[i];
    }

    /*
    while (!VToCheck.empty())
    {
        if (stronglyConnectedComponent(numOfV, VToCheck, inputAdjacencyMatrix.adjacencyMatrix).size() > 1)
        {
            //заменить сильно связную компоненту на одну вершину
            // V.push_back(V.size() + 1);
        }
        for (int i : stronglyConnectedComponent(numOfV, VToCheck, inputAdjacencyMatrix.adjacencyMatrix))
        {
            auto it = remove(VToCheck.begin(), VToCheck.end(), i);
            VToCheck.erase(it, VToCheck.end());
        }
    }*/

    if (VFirst(numOfU, U).size() > 1)
    {
        //создать источник
        /*reallocUp1D(inputV, numOfV, numOfV + 1);
        numOfV += 1;
        inputV[numOfV] = inputV[numOfV - 1] + 1;
        int tmp = numOfU;
        for (int i : VFirst(numOfU, U))
        {
            reallocUp2D(inputU, tmp, tmp + 1);
            reallocUp1D(inputUWeights, tmp, tmp + 1);
            tmp += 1;
            inputU[tmp][0] = inputV[numOfV];
            inputU[tmp][1] = i;
            inputUWeights[tmp] = 0;
        }
        numOfU = tmp;*/
    }

    if (VLast(numOfU, U).size() > 1)
    {
        //создать сток
        /*reallocUp1D(inputV, numOfV, numOfV + 1);
        numOfV += 1;
        inputV[numOfV] = inputV[numOfV - 1] + 1;
        int tmp = numOfU;
        for (int i : VLast(numOfU, U))
        {
            reallocUp2D(inputU, tmp, tmp + 1);
            reallocUp1D(inputUWeights, tmp, tmp + 1);
            tmp += 1;
            inputU[tmp][0] = i;
            inputU[tmp][1] = inputV[numOfV];
            inputUWeights[tmp] = 0;
        }
        numOfU = tmp;*/
    }

    // network

    std::ofstream network("network.txt", std::ios::out);
    if (network.is_open())
    {
        network << "Network (vertexes):" << std::endl;
        for (size_t i = 0; i < numOfV; i++)
        {
            network << V[i] << " ";
        }
        network << std::endl
                << std::endl;
        network << "Network (edges - edges's weights):" << std::endl;
        for (size_t i = 0; i < numOfU; i++)
        {
            network << "(" << U[i][0] << " " << U[i][1] << ") - " << UWeights[i] << std::endl;
        }
    }
    network.close();

    // PERT

    int inf = 1;
    int **distanceMatrix = new int *[numOfV];
    for (size_t i = 0; i < numOfV; i++)
    {
        distanceMatrix[i] = new int[numOfV];
    }

    for (size_t i = 0; i < numOfV; i++)
    {
        for (size_t j = 0; j < numOfV; j++)
        {
            distanceMatrix[i][j] = inf;
        }
    }

    for (size_t i = 0; i < numOfU; i++)
    {
        distanceMatrix[inputU[i][0] - 1][inputU[i][1] - 1] = (-1) * inputUWeights[i];
    }

    for (size_t i = 0; i < numOfV; i++)
    {
        distanceMatrix[i][i] = 0;
    }

    for (size_t k = 0; k < numOfV; k++)
    {
        for (size_t i = 0; i < numOfV; i++)
        {
            for (size_t j = 0; j < numOfV; j++)
            {
                if (distanceMatrix[i][k] < inf && distanceMatrix[k][j] < inf)
                    if (distanceMatrix[i][k] + distanceMatrix[k][j] < distanceMatrix[i][j])
                        distanceMatrix[i][j] = distanceMatrix[i][k] + distanceMatrix[k][j];
            }
        }
    }

    for (size_t i = 0; i < numOfV; i++)
    {
        for (size_t j = 0; j < numOfV; j++)
        {
            distanceMatrix[i][j] = (-1) * distanceMatrix[i][j];
        }
    }

    std::vector<int> criticalPath;
    criticalPath.insert(criticalPath.begin(), inputU[numOfU - 1][1]);
    int pathValue, VPrevious;
    while (criticalPath[0] != 1)
    {
        pathValue = -inf, VPrevious = -inf;
        for (size_t i = numOfU; i > 0; i--)
        {
            if (inputU[i - 1][1] == criticalPath[0])
            {
                if (pathValue < distanceMatrix[inputU[i - 1][0] - 1][inputU[i - 1][1] - 1])
                {
                    pathValue = distanceMatrix[inputU[i - 1][0] - 1][inputU[i - 1][1] - 1];
                    VPrevious = inputU[i - 1][0];
                }
            }
        }
        criticalPath.insert(criticalPath.begin(), VPrevious);
    }

    int *delay = new int[numOfU];
    for (size_t i = 0; i < numOfV; i++)
    {
        for (size_t j = 0; j < numOfU; j++)
        {
            if (U[j][1] == V[i])
            {
                delay[j] = distanceMatrix[0][U[j][1] - 1] - distanceMatrix[0][U[j][0] - 1] - inputUWeights[j];
            }
        }
    }

    // critical path

    std::ofstream PERTPath("PERT.txt", std::ios::out);
    if (PERTPath.is_open())
    {
        PERTPath << "Critical path (vertexes): ";
        for (size_t i = 0; i < criticalPath.size(); i++)
        {
            PERTPath << criticalPath[i] << " ";
        }
        PERTPath << std::endl;
    }
    PERTPath.close();

    // task 1

    std::ofstream PERTLengthOfPath("PERT.txt", std::ios::app);
    if (PERTLengthOfPath.is_open())
    {
        PERTLengthOfPath << std::endl;
        PERTLengthOfPath << "Length of critical path: " << distanceMatrix[0][numOfV - 1] << std::endl;
    }
    PERTLengthOfPath.close();

    // task 2

    std::ofstream PERTMomentsOfEvents("PERT.txt", std::ios::app);
    if (PERTMomentsOfEvents.is_open())
    {
        PERTMomentsOfEvents << std::endl;
        PERTMomentsOfEvents << "Moments of events (vertex - moment): " << std::endl;
        for (size_t i = 0; i < numOfV; i++)
        {
            PERTMomentsOfEvents << inputV[i] << " - "
                                << distanceMatrix[0][i] << std::endl;
        }
    }
    PERTMomentsOfEvents.close();

    // task 3

    std::ofstream PERTEdgeDelays("PERT.txt", std::ios::app);
    if (PERTEdgeDelays.is_open())
    {
        PERTEdgeDelays << std::endl;
        PERTEdgeDelays << "Delays of edges (edge - delay): " << std::endl;
        for (size_t i = 0; i < numOfU; i++)
        {
            PERTEdgeDelays << "(" << inputU[i][0] << " " << inputU[i][1] << ")"
                           << " - "
                           << delay[i] << std::endl;
        }
    }
    PERTEdgeDelays.close();

    delete[] V;
    delete[] UWeights;
    delete[] VToCheck;
    for (size_t i = 0; i < numOfV; i++)
    {
        delete[] U[i];
        delete[] distanceMatrix[i];
    }
    delete[] U;
    delete[] distanceMatrix;
    delete[] delay;

    return 0;
}

int *readingV(int &numOfV, int *V)
{
    std::ifstream input("courseWork_V.txt");
    if (!input.eof())
    {
        for (size_t i = 0; i < numOfV; i++)
        {
            input >> V[i];
        }
        input.close();
    }
    return V;
}
int **readingU(int &numOfU, int **U)
{
    std::ifstream input("courseWork_U.txt");
    if (!input.eof())
    {
        for (size_t i = 0; i < numOfU; i++)
        {
            for (size_t j = 0; j < 2; j++)
            {
                input >> U[i][j];
            }
        }
        input.close();
    }
    return U;
}
int *readingUWeights(int &numOfU, int *UWeights)
{
    std::ifstream inputU("courseWork_UWeight.txt");
    if (!inputU.eof())
    {
        for (size_t i = 0; i < numOfU; i++)
        {
            inputU >> UWeights[i];
        }
        inputU.close();
    }
    return UWeights;
}
size_t countNumOfV()
{
    size_t numOfV = 0;
    std::ifstream input("courseWork_V.txt");
    if (!input.bad())
    {
        std::string line;
        while (getline(input, line))
        {
            numOfV++;
        }
    }
    input.close();
    return numOfV;
}
size_t countNumOfU()
{
    size_t numOfU = 0;
    std::ifstream input("courseWork_U.txt");
    if (!input.bad())
    {
        std::string line;
        while (getline(input, line))
        {
            numOfU++;
        }
    }
    input.close();
    return numOfU;
}

//просмотреть
std::vector<int> stronglyConnectedComponent(int &numOfV, std::vector<int> &VToCheck, int **adjacencyMatrix)
{
    std::vector<int> component;
    for (size_t i = 0; i < numOfV; i++)
    {
        for (size_t j = 0; j < numOfV; j++)
        {
            // find strongly connected component
        }
    }
    return component;
}

//проверить
std::vector<int> VFirst(int &numOfU, int **U)
{
    int flag = 0;
    std::vector<int> VFirst;
    for (size_t i = 0; i < numOfU; i++)
    {
        flag = 0;
        for (size_t j = 0; j < numOfU; j++)
        {
            if (U[i][0] == U[j][1])
            {
                flag = 1;
            }
        }
        if (flag == 0 && std::find(VFirst.begin(), VFirst.end(), i) != VFirst.end())
        {
            VFirst.push_back(i);
        }
    }
    return VFirst;
}

//проверить
std::vector<int> VLast(int &numOfU, int **U)
{
    int flag = 0;
    std::vector<int> VLast;
    for (int i = 0; i < numOfU; i++)
    {
        flag = 0;
        for (size_t j = 0; j < numOfU; j++)
        {
            if (U[i][1] == U[j][0])
            {
                flag = 1;
            }
        }

        if (flag == 0 && std::find(VLast.begin(), VLast.end(), i) != VLast.end())
        {
            VLast.push_back(i);
        }
    }
    return VLast;
}

void reallocUp2D(int **matrix, int size, int newSize)
{
    int **newMatrix = new int *[2];
    for (size_t i = 0; i < newSize; i++)
    {
        newMatrix[i] = new int[newSize];
    }
    size = size < newSize ? size : newSize;
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            newMatrix[i][j] = matrix[i][j];
        }
    }
    matrix = newMatrix;
    for (size_t i = 0; i < size; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}
void reallocUp1D(int *matrix, int size, int newSize)
{
    int *newMatrix = new int[newSize];
    size = size < newSize ? size : newSize;
    for (size_t i = 0; i < size; i++)
    {
        newMatrix[i] = matrix[i];
    }
    matrix = newMatrix;
    delete[] matrix;
}
