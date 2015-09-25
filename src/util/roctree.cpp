#include <AOFlagger/util/roctree.h>

#include <iostream>

ROCTree::ROCTree(long double parameterRootValue, ROCTreeClassifier &classifier)
 : _tree(0), _parameterRootValue(parameterRootValue), _classifier(classifier)
{
}

ROCTree::~ROCTree()
{
}

ROCTree::ROCTreeNode *ROCTree::Find(ROCTreeNode **node, long double parameter, long double stepSize, long double falseRatioValue, long double falseRatioPrecision, int depth)
{
	if((*node) == 0)
	{
		ROCTreeNode *n = new ROCTreeNode();
		n->parameter = parameter;
		n->left = 0;
		n->right = 0;
		_classifier.Perform(n->falseRatio, n->trueRatio, parameter);
		*node = n;
	}
	depth++;
	if(depth == 20) {
		return *node;
	} 
	// Are we within the desired precision?
	if((*node)->falseRatio - falseRatioPrecision <= falseRatioValue && (*node)->falseRatio + falseRatioPrecision >= falseRatioValue)
	{
		std::cout << "+ found (" << depth << ",threshold=" << (*node)->parameter << ",searching=" << falseRatioValue << ",current=" << (*node)->falseRatio << ")" << std::endl;
		// Precision reached
		return *node;
	} else {
		// Precision not reached, traverse down the tree in the proper direction
		if((*node)->falseRatio < falseRatioValue)
		{
			std::cout << "<-- left traversel (" << depth << ",threshold=" << (*node)->parameter << ",searching=" << falseRatioValue << ",current=" << (*node)->falseRatio << ")" << std::endl;
			return Find(&(*node)->right, parameter-stepSize, stepSize/2.0L, falseRatioValue, falseRatioPrecision, depth);
		}
		else
		{
			std::cout << "--> right traversel (" << depth << ",threshold=" << (*node)->parameter << ",searching=" << falseRatioValue << ",current=" << (*node)->falseRatio << ")" << std::endl;
			return Find(&(*node)->left, parameter+stepSize, stepSize/2.0L, falseRatioValue, falseRatioPrecision, depth);
		}
	}
}

void ROCTree::Save(const std::string &rocFilename)
{
	std::ofstream rocFile(rocFilename.c_str());
	if(_tree != 0)
		SaveNode(rocFile, *_tree);
	rocFile.close();
}

void ROCTree::SaveNode(std::ofstream &file, ROCTree::ROCTreeNode &node)
{
	if(node.left != 0)
		SaveNode(file, *node.left);
	file
		<< "\t" << node.falseRatio*100.0L
		<< "\t" << node.trueRatio*100.0L
		<< "\t" << node.parameter
		<< std::endl;
	if(node.right != 0)
		SaveNode(file, *node.right);
}
