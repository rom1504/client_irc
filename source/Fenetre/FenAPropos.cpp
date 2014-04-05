#include "FenAPropos.h"

FenAPropos::FenAPropos() : QDialog()
{
	setModal(true);
	setMinimumSize(200,200);
	setWindowTitle(tr("A propos de client_irc."));
	setWindowIcon(QIcon(":images/icone.png"));
	m_layoutPrincipal=new QHBoxLayout;
	m_labelLogo=new QLabel;
	m_logo=new QPixmap(":images/grande_icone.png");
	m_labelLogo->setPixmap(*m_logo);
	m_layoutPrincipal->addWidget(m_labelLogo);
	m_labelDescription=new QLabel(tr("<h1>client_irc</h1><p>Client irc multiplatforme</p><p>Version 0.3<br />Créé par Romain Beaumont aka rom1504<br />Icone du programme réalisé par Hugo : http://www.cadmium-production.com/<br />Page sur mon portfolio traitant de ce programme : http://rom1504.no-ip.info/portfolio/index.php?page=20<br />Compilé le 17/08/2009<br />Ce programme est sous licence gpl ( vous trouverez cette licence dans le dossier de ce programme )</p><p>Copyright (C) 2008-2009 Romain Beaumont</p>"));
	m_layoutPrincipal->addWidget(m_labelDescription);
	setLayout(m_layoutPrincipal);
	show();
}

