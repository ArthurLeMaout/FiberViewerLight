#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "QVTKWidget.h"
#include "FiberViewerLightGUI.h"
#include "FiberViewerLightCLP.h"


int main(int argc, char* argv[])
{
	PARSE_ARGS;
	QApplication app(argc, argv);
	if(!nogui)
	{
		
		
		FiberViewerLightGUI* GUIWin=new FiberViewerLightGUI(nogui,input,output);
		GUIWin->show();
		
		return app.exec();
	}
	else
	{
		FiberViewerLightGUI* GUIWin=new FiberViewerLightGUI(nogui);
		if(input!="" && process_list.size()>0)
			GUIWin->ProcessWithoutGUI(input,output,process_list,use_danielsson_transform);
		else
			std::cout<<"One or more required parameters are missing."<<std::endl;
		return 0;
	}
}
