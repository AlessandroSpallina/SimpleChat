/****************************************************************************
-* Copyright © 2017 Alessio Greco
-* github: https://github.com/RootPOI
-*
-* This program is free software: you can redistribute it and/or modify
-* it under the terms of the GNU General Public License as published by
-* the Free Software Foundation, either version 3 of the License, or
-* (at your option) any later version.
-*
-* This program is distributed in the hope that it will be useful,
-* but WITHOUT ANY WARRANTY; without even the implied warranty of
-* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-* GNU General Public License for more details.
-*
-* You should have received a copy of the GNU General Public License
-* along with this program. If not, see <http://www.gnu.org/licenses/>.
-****************************************************************************/
/*--------------------Prototype Function Client-----------------------------*/
#include "FunctionClient.h"
void FFLUSH(void);							//Pulizia Buffer STDIN (Prof.Cavalieri 2015/2016)
void LDS (char s[], unsigned short dim);	//Lettura Stringhe Pulita (Prof.Cavalieri 2015/2016)
const char* Hours (void);					//Ora, Restituisce una stringa del tipo hh:mm:ss (Greco Alessio 2016/2017)
void *ClientThreadMSG (void *arg);			//Thread Che Gestisce La Chat del Client (Greco Alessio 2016/2017)
/*--------------------------------------------------------------------------*/
