#ifndef DEF_FENAPROPOS
#define DEF_FENAPROPOS
 
#include <QtGui>

class FenAPropos : public QDialog
{
	Q_OBJECT
	 
	public:
	FenAPropos();
	
	private:
	QHBoxLayout * m_layoutPrincipal;
	QLabel *m_labelLogo;
	QPixmap * m_logo;
	QLabel * m_labelDescription;
};
 
#endif
