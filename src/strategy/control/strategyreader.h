#ifndef RFISTRATEGYREADER_H
#define RFISTRATEGYREADER_H

#include <stdexcept>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>

namespace rfiStrategy {

	class StrategyReaderError : public std::runtime_error
	{
		public:
			StrategyReaderError(const std::string &arg) : std::runtime_error(arg) { }
	};

class StrategyReader {
	public:
		StrategyReader();
		~StrategyReader();

		class Strategy *CreateStrategyFromFile(const std::string &filename);
	private:
		class Action *parseChild(xmlNode *node);
		class Strategy *parseStrategy(xmlNode *node);
		class Strategy *parseRootChildren(xmlNode *rootNode);
		void parseChildren(xmlNode *node, class ActionContainer *parent);
		class Action *parseAction(xmlNode *node);

		xmlNode *getTextNode(xmlNode *node, const char *subNodeName, bool allowEmpty = false) const;
		int getInt(xmlNode *node, const char *name) const;
		double getDouble(xmlNode *node, const char *name) const;
		std::string getString(xmlNode *node, const char *name) const;
		bool getBool(xmlNode *node, const char *name) const { return getInt(node,name) != 0; }

		class Action *parseAbsThresholdAction(xmlNode *node);
		class Action *parseAdapter(xmlNode *node);
		class Action *parseAddStatistics(xmlNode *node);
		class Action *parseBaselineSelectionAction(xmlNode *node);
		class Action *parseCalibratePassbandAction(xmlNode *node);
		class Action *parseChangeResolutionAction(xmlNode *node);
		class Action *parseCombineFlagResults(xmlNode *node);
		class Action *parseCutAreaAction(xmlNode *node);
		class Action *parseDirectionalCleanAction(xmlNode *node);
		class Action *parseDirectionProfileAction(xmlNode *node);
		class Action *parseEigenValueVerticalAction(xmlNode *node);
		class Action *parseForEachBaselineAction(xmlNode *node);
		class Action *parseForEachComplexComponentAction(xmlNode *node);
		class Action *parseForEachMSAction(xmlNode *node);
		class Action *parseForEachPolarisationBlock(xmlNode *node);
		class Action *parseFourierTransformAction(xmlNode *node);
		class Action *parseFrequencyConvolutionAction(xmlNode *node);
		class Action *parseFrequencySelectionAction(xmlNode *node);
		class Action *parseFringeStopAction(xmlNode *node);
		class Action *parseHighPassFilterAction(xmlNode *node);
		class Action *parseImagerAction(xmlNode *node);
		class Action *parseIterationBlock(xmlNode *node);
		class Action *parseNormalizeVarianceAction(xmlNode *node);
		class Action *parsePlotAction(xmlNode *node);
		class Action *parseQuickCalibrateAction(xmlNode *node);
		class Action *parseSetFlaggingAction(xmlNode *node);
		class Action *parseSetImageAction(xmlNode *node);
		class Action *parseSlidingWindowFitAction(xmlNode *node);
		class Action *parseStatisticalFlagAction(xmlNode *node);
		class Action *parseSVDAction(xmlNode *node);
		class Action *parseSumThresholdAction(xmlNode *node);
		class Action *parseTimeConvolutionAction(xmlNode *node);
		class Action *parseTimeSelectionAction(xmlNode *node);
		class Action *parseUVProjectAction(xmlNode *node);
		class Action *parseWriteDataAction(xmlNode *node);
		class Action *parseWriteFlagsAction(xmlNode *node);

		xmlDocPtr _xmlDocument;

		static int useCount;
};

}

#endif
