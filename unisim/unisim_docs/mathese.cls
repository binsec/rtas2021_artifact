\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{mathese}[]
% version 1.0 Olivier Pothier 1999/09/15
% version 1.1 Catherine Lamy 2000/04/18

%---------------------------------------
%---- on se base sur la classe book ----
%---- sur papier a4, 2 faces, 1 col ----  
%---------------------------------------
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{book}}
\ProcessOptions
\LoadClass{book}

%----------------------
%---- les packages ----
%----------------------
%\usepackage[dvips]{color,graphicx} % removed dvips option
\usepackage{color}
\usepackage{graphicx}
\usepackage{epsfig}
\usepackage[francais,english]{babel}
%\usepackage[T1]{fontenc}
\usepackage{euscript}
\usepackage{battail} 
\usepackage{fancyhdr}
\usepackage{latexsym}
%\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{amsmath}
\usepackage{amsfonts}
%\usepackage[pdfauthor={David Parello},pdftitle={Rapport d'activites},pdftex]{hyperref}
%\usepackage{progSTF,colorSTF}
%\usepackage{myprogSTF,colorSTF}
%\usepackage{pstricks,graphics} % removed pstricks
\usepackage{graphics}
%\usepackage[pdf]{pstricks}
%\usepackage{progSTF}
\usepackage{subfigure}

\usepackage[normalem]{ulem} % normalem garantit que \em reste l'italique
\usepackage{rotating}
\usepackage{verbatim} 

%------------------------------
%---- la taille de la page ----
%------------------------------
\voffset 0.0cm
\hoffset 0.0cm
\textheight 24cm
\textwidth 16cm
\topmargin 0.0cm
\oddsidemargin 0.0cm
\evensidemargin 0.0cm

%----------------------------------------------------------------
%---- pour la sombre histoire que \chapter* ne fait pas appel
%---- � chaptermark, on a toujours \leftmark qui est en 
%---- majuscules (chiant au possible, on remarque au passage
%---- que T. Oetiker, dans ``The Not So Short Introduction to
%---- Latex 2e'' a laiss� tomb�...). On definit donc un ersazt
%---- de \chapter*, \chapternonum, qui fait appel � \markboth
%---- avec deux arguments identiques (notons au passage que 
%---- \chapter, lui, comme un con, ne passe qu'un argument, le
%---- premier, � markboth au traver de \chaptermark. Il faudrait
%---- aussi r��crire \chapter, demain peut-�tre :-))
%----------------------------------------------------------------
\newcommand{\chapternonum}[1]{\chapter*{#1\markboth{#1}{#1}}}

%-----------------------------------
%---- on se charge des headings ----
%-----------------------------------
\pagestyle{fancy}
%\bibliographystyle{fancy}
%---- on doit redefinir \chaptermark et \sectionmark pour qu'ils
%---- ne mettent pas tout en majuscule. On vire au passage tous les
%---- tests d�biles fait dans leur version originale, et on corrige
%---- le fait que \chaptermark ne passait qu'un seul argument �
%---- markboth, ce qui posait probl�me dans certains cas, rares il est
%---- vrai.
%---- Je ne veux pas afficher \chaptername dans le header, et on
%---- laisse tout dans la casse voulue par l'auteur... L'argument
%---- pass� par \chapter � \chaptermark est son argument propre (�
%---- savoir le nom du chapitre. On fait de m�me pour section.
\renewcommand{\chaptermark}[1]{\markboth {\thechapter.\ #1}{\thechapter.\ #1}}
\renewcommand{\sectionmark}[1]{\markright{\thesection. \ #1}}
\fancyhf{}
%----       le num�ro de la page       ----
%---- � gauche sur les pages de gauche ----
%---- � droite sur les pages de droite ----
\fancyhead[LE,RO]{\thepage}
%---- \leftmark, cad le chapitre, � droite des pages de gauche
\fancyhead[RE]{\textsc{\leftmark}}
%---- \rightmark, cad la section, � gauche des pages de droite
\fancyhead[LO]{\textsc{\rightmark}}
%---- les initiales ----%
%\fancyfoot[RE,LO]{\footnotesize C.L.}
%---- la version ----%
%\fancyfoot[CE,CO]{\footnotesize version finale}
%---- la date ----%
%\fancyfoot[LE,RO]{\tiny 2000}
%---- les barres ----%
\renewcommand{\headrulewidth}{1pt}
\renewcommand{\footrulewidth}{1pt}
\addtolength{\headheight}{1pt}
%---- on redefinit le style plain (\chapter le force pour la
%---- premi�re page de tout chapitre
\fancypagestyle{plain}{
  \fancyhead[RE]{}
  \fancyhead[LO]{}
}



%-----------------------------------------------------------
% DAVID PARELLO : Ma Configuration, mes nouvelles Commandes
%-----------------------------------------------------------

\newcommand{\REM}[1]{
\begin{center}
\begin{tabular} {@{} p{12cm} @{}}
{\it Note~:}\\
\hline
%{\it \textcolor{mauve}{#1}}\\
{\it #1}\\
\hline
\end{tabular}
\end{center}
}


%\DeclareFixedFont{\mybf}{OT1}{cmtt}{bx}{it}{9}

\newcommand{\mycaption}[1]{\caption{\textit{#1}}}

%\def\HLC{\textit\textcolor{blue}}
%\newcommand{\HLC}[1]{\textit{\textcolor{blue}{#1}}}
\newcommand{\HLC}[1]{\textit{\textcolor{blue}{\underline{#1}}}}

% PARGRAPH
%\setlength{\parskip}{1ex plus 0.5ex minus 0.2ex}

%\def\nom
\def\applu{\textsc{applu}}
\def\swim{\textsc{swim}}
\def\equake{\textsc{equake}}
\def\wupwise{\textsc{wupwise}}
\def\mgrid{\textsc{mgrid}}
\def\galgel{\textsc{galgel}}
\def\facerec{\textsc{facerec}}
\def\ammp{\textsc{ammp}}
\def\fmad{\textsc{fmad}}
\def\apsi{\textsc{apsi}}
\def\mesa{\textsc{mesa}}
\def\art{\textsc{art}}


%\def\{\textsc{}}
\def\ANO{\texttt}
\def\CODE{\mathnormal}
\def\X{$\surd$}
\def\TRG{$\blacktriangleright$}
\def\TX{\textrm}
\def\SX{\scriptsize}
\def\sout{\emph}
\def\tbf{\textbf}
\def\ano{\texttt}
\def\code{\texttt}

\def\O{$\mathcal{O}$}
\def\A{$\mathcal{A}$}
\def\G{$\mathcal{G}$}

\definecolor{gris}{rgb}{0.9, 0.9, 0.9} 
\newcommand{\CB}[1]{\colorbox{gris}{#1}}
%\definecolor{gris}{gray}{0.9} 

%\definecolor{gris}{gray}{0.9} 


\newcommand{\PROC}[1]{\textsf{\textit{#1}}}
\newcommand{\ROT}[1]{\begin{turn}{45}#1\end{turn}}
\newcommand{\VROT}[1]{\begin{turn}{90}#1\end{turn}}
%\newcommand{\MROT}[1]{\multicolumn{1}{@{\;}c@{\;}}{\begin{turn}{50}\small\rlap{#1}\end{turn}}}
%\newcommand{\MROT}[1]{\multicolumn{1}{@{\;}c@{\;}}{\begin{turn}{50}\footnotesize\rlap{#1}\end{turn}}}
\newcommand{\MROT}[1]{\multicolumn{1}{@{\;}c@{\;}}{\rotatebox[origin=lt]{50}{\footnotesize\rlap{#1}}}}

%\newcommand{\MROT}[1]{\multicolumn{1}{@{\;}c@{\;}}{\begin{turn}{60}\small\rlap{#1}\end{turn}}}
%\newcommand{\MROT}[1]{\multicolumn{1}{@{\;}c@{\;}}{\rotatebox{40}{\small\rlap{#1}}}}
%\newcommand{\MROT}[1]{\multicolumn{1}{@{\;}c@{\;}}{
%\begin{sideways}
%\rlap{#1}
%\end{sideways}
%}}

%\renewcommand{\section}[1] 
%{ 
%        \vfill 
%        \colorbox{gris} 
%        { 
%                \begin{tabular}{>{\centering}p{\banner}} 
%%                \qquad\large\textbf{#1} \hfill \emph{\footnotesize{#2}} 
%                \qquad\large\textbf{#1} \hfill 
%                \end{tabular} 
%        } 
%%       \vspace{6mm} 
%} 

\newenvironment{script}{\endgraf\footnotesize\verbatim}{\endverbatim}
