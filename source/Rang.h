#ifndef DEF_RANG
#define DEF_RANG

#include <QtGui>

struct SRang
{
	QString rang;
	QString mode;
	QIcon * icone;
	int importance;
};

class Rang
{
	public :
		Rang();
		void ajouter(QString rang,QString mode,QIcon * icone,int importance);
		const SRang rang(QString rang) const;
		const SRang mode(QString mode) const;
		
	private :
		QVector<SRang> m_rang;
		SRang m_srangNul;
};

#endif
