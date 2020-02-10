#pragma once
#include <maya/MStringArray.h>
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>
#include "cylinder.h"
#include "LSystem.h"

MStatus returnStatus;
#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }


class LSystemNode : public MPxNode
{
public:
	LSystemNode() {};
	virtual         ~LSystemNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*   creator();
	static  MStatus initialize();

	static MTypeId  id;
	static MObject  time;
	static MObject  outputMesh;
	static MObject  stepSize;
	static MObject  angle;
	static MObject  grammar;

	LSystem mLSystem;
	
protected:
	MObject createMesh(const MTime& time, MObject& outData, MStatus& stat);
	
};


MObject LSystemNode::time;
MObject LSystemNode::outputMesh;
MTypeId LSystemNode::id(0x80000);
MObject LSystemNode::angle;
MObject LSystemNode::stepSize;
MObject LSystemNode::grammar;

void* LSystemNode::creator()
{
	return new LSystemNode;
}

MStatus LSystemNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numAttr;
	MStatus returnStatus;

	LSystemNode::angle = numAttr.create("angle", "an", MFnNumericData::kDouble, 30.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lSystem angle attribute\n");

	LSystemNode::stepSize = numAttr.create("step", "st", MFnNumericData::kDouble, 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lSystem step size attribute\n");

	LSystemNode::grammar = typedAttr.create("grammar", "grm", MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lSystem grammar attribute\n");

	LSystemNode::time = unitAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode time attribute\n");

	LSystemNode::outputMesh = typedAttr.create("outputMesh", "out", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode output attribute\n");

	typedAttr.setStorable(false);

	returnStatus = addAttribute(LSystemNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = addAttribute(LSystemNode::angle);
	McheckErr(returnStatus, "ERROR adding angle attribute\n");

	returnStatus = addAttribute(LSystemNode::stepSize);
	McheckErr(returnStatus, "ERROR adding step size attribute\n");

	returnStatus = addAttribute(LSystemNode::grammar);
	McheckErr(returnStatus, "ERROR adding grammar attribute\n");


	returnStatus = attributeAffects(LSystemNode::time, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	//returnStatus = attributeAffects(LSystemNode::angle, LSystemNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");

	//returnStatus = attributeAffects(LSystemNode::stepSize, LSystemNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");

	//returnStatus = attributeAffects(LSystemNode::grammar, LSystemNode::outputMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

MObject LSystemNode::createMesh(const MTime& time,
	MObject& outData,
	MStatus& stat)
{
	MFnMesh meshFS;
	MObject newMesh;
	return newMesh;
}


MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	MGlobal::displayInfo("computeExecuted:");
	if (plug == outputMesh) {
		/* Get time */
		MDataHandle timeDataHandle = data.inputValue(time, &returnStatus);
		McheckErr(returnStatus, "Error getting time data handle\n");
		MTime time = timeDataHandle.asTime();

		/* Get angle */
		MDataHandle angleDataHandle = data.inputValue(angle, &returnStatus);
		McheckErr(returnStatus, "Error getting angle handle");
		double nodeAngle = angleDataHandle.asDouble();

		/* Get step size */
		MDataHandle stepDataHandle = data.inputValue(stepSize, &returnStatus);
		McheckErr(returnStatus, "Error getting step size handle");
		double nodeStep = stepDataHandle.asDouble();

		/* Get grammar */
		MDataHandle gramDataHandle = data.inputValue(grammar, &returnStatus);
		McheckErr(returnStatus, "Error getting grammar handle");
		MString nodeGrammar = gramDataHandle.asString();


		/* Get output object */
		MDataHandle meshDataHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		
		// mLSystem.loadProgramFromString(nodeGrammar.asChar());
		mLSystem.loadProgramFromString("F\nF->F[+F]F[-F]F");
		mLSystem.setDefaultAngle(nodeAngle);
		mLSystem.setDefaultStep(nodeStep);
		int frame = (int)time.as(MTime::kFilm);
		int itr = frame % 5;
		std::vector<LSystem::Branch> branches;
		mLSystem.process(itr, branches);

		int numVertices, numFaces;
		float meshSize = 0.5f;
		MPointArray points;
		MIntArray faceFinalCount;
		MIntArray faceFinalConnects;
		MFnMesh meshFS;

		for (int i = 0; i < branches.size(); i++)
		{
			LSystem::Branch curBranch = branches.at(i);
			vec3 first = curBranch.first;
			vec3 second = curBranch.second;
			
			CylinderMesh cyMesh = CylinderMesh(MPoint(first[0], first[2], first[1]), MPoint(second[0], second[2], second[1]));

			cyMesh.appendToMesh(points, faceFinalCount, faceFinalConnects);
		}

		numVertices = points.length();
		numFaces = faceFinalCount.length();

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		// MGlobal::displayInfo("numVertices:");
		// MGlobal::displayInfo(MString(std::to_string(numVertices).c_str()));


		meshFS.create(numVertices, numFaces, points, faceFinalCount, faceFinalConnects, newOutputData, &returnStatus);

		// MGlobal::displayInfo();
		MGlobal::displayInfo(std::to_string(time.as(MTime::Unit::kFilm)).c_str());
		meshDataHandle.set(newOutputData);
		data.setClean(plug);
	}
	else
		return MS::kUnknownParameter;
	return MS::kSuccess;
}