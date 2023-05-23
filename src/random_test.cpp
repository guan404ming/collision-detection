#include <iostream>
#include <vector>
#include <random>
#include <cassert>

// Global Variable
double r = 1;
int count = 0;
int minCoord = 0, maxCoord = 100;

struct Dot
{
    int x;
    int y;
    Dot(int x_, int y_) : x(x_), y(y_) {}
};

struct QuadTreeNode
{
    Dot LT;
    Dot RB;
    QuadTreeNode *nodes[4];
    std::vector<Dot> dots;
    QuadTreeNode(Dot LT_, Dot RB_) : LT(LT_), RB(RB_)
    {
        for (int i = 0; i < 4; ++i)
            nodes[i] = nullptr;
    }
};

void constructQuadTree(std::vector<Dot> dots, QuadTreeNode *root)
{
    std::vector<Dot> dots_ = dots;
    while (root->dots.size() < 4 && !dots_.empty())
    {
        root->dots.push_back(dots_.front());
        dots_.erase(dots_.begin());
        ;
    }

    // Create child nodes
    int centerX = (root->LT.x + root->RB.x) / 2;
    int centerY = (root->LT.y + root->RB.y) / 2;

    if (!root->nodes[0])
    {
        root->nodes[0] = new QuadTreeNode{root->LT, Dot(centerX, centerY)};
        root->nodes[1] = new QuadTreeNode{Dot(centerX + 1, root->LT.y), Dot(root->RB.x, centerY)};
        root->nodes[2] = new QuadTreeNode{Dot(root->LT.x, centerY + 1), Dot(centerX, root->RB.y)};
        root->nodes[3] = new QuadTreeNode{Dot(centerX + 1, centerY + 1), root->RB};
    }

    // for (auto i : root->dots)
    //     std::cout << "(" << i.x << " " << i.y << ") ";
    // std::cout << "\n";

    if (!dots_.empty())
    {
        for (auto i : dots_)
        {
            // std::cout << "(" << i.x << " " << i.y << ") ";
            int pos = (i.x > centerX) + 2 * (i.y > centerY);
            constructQuadTree(std::vector<Dot>{i}, root->nodes[pos]);
        }
    }
}

void printQuadTree(QuadTreeNode *root)
{
    for (auto i : root->dots)
    {
        std::cout << "(" << i.x << ", " << i.y << "), ";
    }
    std::cout << ((root->dots.size() == 0) ? "" : "\n");
    if (root->nodes[0])
    {
        for (auto i : root->nodes)
        {
            printQuadTree(i);
        }
    }
}

bool check(Dot A, Dot B)
{
    int dx = A.x - B.x;
    int dy = A.y - B.y;
    double distance = std::sqrt(dx * dx + dy * dy);
    count++;

    // print the answer
    // if (distance < 2 * r)
    // {
    //     std::cout << "(" << A.x << ", " << A.y << ")"
    //               << " "
    //               << "(" << B.x << ", " << B.y << ")\n";
    // }

    return distance < 2 * r;
}

bool checkQuadTree(const QuadTreeNode *node)
{
    if (node->dots.size() <= 1)
    {
        return false;
    }

    // Check collisions within the dots of the current node
    if (node->dots.size() != 0)
    {
        for (size_t i = 0; i < node->dots.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < node->dots.size(); ++j)
            {
                check(node->dots[i], node->dots[j]);
            }
        }
    }
    for (auto D : node->dots)
    {
        for (auto n : node->nodes)
        {
            for (auto d : n->dots)
            {
                check(D, d);
            }
        }
    }

    // Recursively check collisions within child nodes
    for (int i = 0; i < 4; ++i)
    {
        if (node->nodes[i])
        {
            checkQuadTree(node->nodes[i]);
        }
    }

    return false;
}

std::vector<Dot> generateDots(int minCoord, int maxCoord, int minLength, int maxLength)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> lengthDist(minLength, maxLength);

    int length = lengthDist(gen);
    std::uniform_int_distribution<int> coordDist(minCoord, maxCoord);

    std::vector<Dot> dots;
    for (int i = 0; i < length; ++i)
    {
        int x = coordDist(gen);
        int y = coordDist(gen);
        dots.emplace_back(x, y);
    }

    // print dots
    // for (auto i : dots)
    // {
    //     std::cout << "Dot: (" << i.x << ", " << i.y << ")\n";
    // }

    return dots;
}

void randomTest(int minLength, int maxLength, int testAmount)
{
    if (minLength * maxLength < 0 || testAmount < 0) return;
    
    for (size_t i = minLength; i < maxLength + 1; i++)
    {
        int basicSum = 0, quadSum = 0;
        for (size_t j = 0; j < testAmount; j++)
        {
            std::vector<Dot> input = generateDots(minCoord, maxCoord, i, i);

            // QuadTree
            QuadTreeNode *root = new QuadTreeNode{Dot{0, maxCoord}, Dot{maxCoord, 0}};
            constructQuadTree(input, root);
            count = 0;
            checkQuadTree(root);
            quadSum += count;

            // Basic
            count = 0;
            for (size_t i = 0; i < input.size() - 1; i++)
            {
                for (size_t j = i + 1; j < input.size(); j++)
                {
                    check(input[i], input[j]);
                }
            }
            basicSum += count;
        }
        std::cout << basicSum / 10000 << "\n";
        std::cout << quadSum / 10000 << "\n";
    }
}

int main()
{
    randomTest(1, 100, 10000); // output.csv
    randomTest(1, 10, 10000); // output2.csv
    return 0;
}
