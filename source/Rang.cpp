#include "Rang.h"

Rang::Rang()
{
	m_srangNul.rang="";
	m_srangNul.mode="";
	m_srangNul.icone=new QIcon();
	m_srangNul.importance=0;
}

void Rang::ajouter(QString rang,QString mode,QIcon * icone,int importance)
{
	SRang srang;
	srang.rang=rang;
	srang.mode=mode;
	srang.icone=icone;
	srang.importance=importance;
	m_rang<<srang;
}

const SRang Rang::rang(QString rang) const
{
	for(int i=0;i<m_rang.size();i++) if(m_rang[i].rang==rang) return m_rang[i];
	return m_srangNul;
}

const SRang Rang::mode(QString mode) const
{
	for(int i=0;i<m_rang.size();i++) if(m_rang[i].mode==mode) return m_rang[i];
	return m_srangNul;
}
