#include <QApplication>
#include "common/simpleViewer.hpp"

int main(int argc, char** argv)
{
	// Read command lines arguments.
	QApplication application(argc,argv);

	  // Instantiate the viewer.
	Viewer viewer;

	viewer.setWindowTitle("DepthMapToPointCloud");

	  // Make the viewer window visible on screen.
	viewer.show();

	  // Run main loop.
	return application.exec();

	return 0;
}

