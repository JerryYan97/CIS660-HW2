#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>
#include "Util.h"
#include "LSystemCmd.h"
#include "LSystemNode.h"

MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "MyPlugin", "1.0", "Any");

    // Register Command
    status = plugin.registerCommand( "LSystemCmd", LSystemCmd::creator, LSystemCmd::newSyntax );
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

	// Register Node
	status = plugin.registerNode("LSystemNode", LSystemNode::id, LSystemNode::creator, LSystemNode::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	// Pop up GUI:
	std::cout.flush();
	MGlobal::executeCommand(cmd.c_str());

	// NOTE: There are always too many bugs by using this way. Thus I decide
	// to abandon this method. My command cannot find corresponding 'proc' in this way.
	// char buffer[2048];
	// sprintf_s(buffer, 2048, "source \"%s/MyPluginDialog.mel\";", plugin.loadPath().asChar());
	// MGlobal::executeCommand(buffer, true);

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

	// Deregister Command:
    status = plugin.deregisterCommand( "LSystemCmd" );
    if (!status) {
	    status.perror("deregisterCommand");
	    return status;
    }

	// Deregister Node:
	status = plugin.deregisterNode(LSystemNode::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}


    return status;
}


