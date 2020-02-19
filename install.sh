#!/bin/bash

echo "> Instalando..."

if cp libarq.a /usr/lib && cp src/arq.h /usr/include 
then
	echo "> Instalação concluida."
else
	echo "> Instalação mal sucedida."
fi
