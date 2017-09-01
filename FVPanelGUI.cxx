#include "FVPanelGUI.h"

FVPanelGUI::FVPanelGUI(QWidget* Parent, FiberDisplay* FVDisplay) : QWidget(Parent)
{
	m_Display=FVDisplay;
	m_PB_Undo=new QPushButton("Undo", this);
	m_PB_Next=new QPushButton("Next", this);
}
