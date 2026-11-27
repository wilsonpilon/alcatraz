#include "msxgl.h"
#include "dos.h"
#include "diagnostic.h"
#include "disprosoft.h"
#include "opening.h"

void main(void)
{
	DOS_Version version = { 0, 0 };

	if (DOS_GetVersion(&version) < 2 || (version.Kernel >> 8) < 2)
	{
		DOS_StringOutput("ALCATRAZ requer MSX-DOS 2.\r\n$");
		DOS_Exit(1);
		return;
	}
	if (Sys_GetMSXVersion() < 2)
	{
		DOS_StringOutput("ALCATRAZ requer MSX 2+ ou superior.\r\n$");
		DOS_Exit(1);
		return;
	}
	if (!Disprosoft_Show())
	{
		BIOS_InterSlotCall(g_EXPTBL[0], R_INITXT);
		DOS_StringOutput("Falha ao desenhar a abertura Disprosoft.\r\n$");
		DOS_Exit(1);
		return;
	}
	if (!Diagnostic_Run())
		return;
	BIOS_Exit(Opening_Show());
}
