#include "LSystemCmd.h"

#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include "LSystem.h"
#include <list>
#include <string>

const char *stepSizeShortFlag = "-sz", *stepSizeLongFlag = "-stepsize";
const char *angleShortFlag = "-a", *angleLongFlag = "-angle";
const char *grammarShortFlag = "-g", *grammarLongFlag = "-grammar";
const char *iterationsShortFlag = "-i", *iterationsLongFlag = "-iterations";
std::string curvePrefix = "curve -d 1 ";
std::string curvePostfix = "-k 0 -k 1;";


/*
void genCurveCmd(std::string& iStr, std::vector<LSystem::Branch>& iBran)
{
	for(int i = 0; i < )
}
*/

LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

MStatus LSystemCmd::doIt( const MArgList& args )
{
	MString mStepSize;
	MString mAngle; // In degree.
	MString mGrammar; 
	MString mIterations;
	// message in Maya output window
	cout<<"LSystemCmd is loaded successfully!"<<endl;
	std::cout.flush();

	// message in scriptor editor
	MGlobal::displayInfo("LSystemCmd is loaded successfully!");

	MArgDatabase argData(syntax(), args);
	if (argData.isFlagSet(stepSizeShortFlag))
	{
		argData.getFlagArgument(stepSizeShortFlag, 0, mStepSize);
	}
	if (argData.isFlagSet(angleShortFlag))
	{
		argData.getFlagArgument(angleShortFlag, 0, mAngle);
	}
	if (argData.isFlagSet(grammarShortFlag))
	{
		argData.getFlagArgument(grammarShortFlag, 0, mGrammar);
	}
	if (argData.isFlagSet(iterationsShortFlag))
	{
		argData.getFlagArgument(iterationsShortFlag, 0, mIterations);
	}

	double sz = mStepSize.asDouble();
	double angle = mAngle.asDouble();
	const char* grammar = mGrammar.asChar();
	int iterations = mIterations.asInt();

	/*MGlobal::displayInfo("Step Size:" + mStepSize);
	MGlobal::displayInfo("Angle:" + mAngle);
	MGlobal::displayInfo("Grammar:" + mGrammar);
	MGlobal::displayInfo("Iterations:" + mIterations);*/
	
	MGlobal::displayInfo(std::to_string(sz).c_str());
	MGlobal::displayInfo(std::to_string(angle).c_str());
	MGlobal::displayInfo(grammar);
	MGlobal::displayInfo(std::to_string(iterations).c_str());

	LSystem mLSystem;
	std::vector<LSystem::Branch> branches;
	mLSystem.loadProgramFromString(grammar);
	mLSystem.setDefaultAngle(angle);
	mLSystem.setDefaultStep(sz);
	mLSystem.process(iterations, branches);
	for (int i = 0; i < branches.size(); i++)
	{
		LSystem::Branch curBranch = branches.at(i);
		vec3 first = curBranch.first;
		vec3 second = curBranch.second;

		std::string firstPos = "-p " + std::to_string(first[0]) + " " + std::to_string(first[2]) + " " + std::to_string(first[1]) + " ";
		std::string secondPos = "-p " + std::to_string(second[0]) + " " + std::to_string(second[2]) + " " + std::to_string(second[1]) + " ";
		std::string mCmd = curvePrefix + firstPos + secondPos + curvePostfix;
		MGlobal::executeCommand(mCmd.c_str());
	}

	// MGlobal::executeCommand("curve -d 1 -p 0 0 0 -p 3 5 6 -k 0 -k 1 -name helloCurve;");

	// message in Maya output window
    // cout<<"Implement Me!"<<endl;
	// std::cout.flush();

	// message in scriptor editor
	// MGlobal::displayInfo("Implement Me!");

    return MStatus::kSuccess;
}

MSyntax LSystemCmd::newSyntax() {
	MSyntax syntax;
	syntax.addFlag(stepSizeShortFlag, stepSizeLongFlag, MSyntax::kDouble);
	syntax.addFlag(angleShortFlag, angleLongFlag, MSyntax::kDouble);
	syntax.addFlag(grammarShortFlag, grammarLongFlag, MSyntax::kString);
	syntax.addFlag(iterationsShortFlag, iterationsLongFlag, MSyntax::kLong);
	return syntax;
}
