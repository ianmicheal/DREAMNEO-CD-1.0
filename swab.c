
/*
######################################################################
#                 Dream neo cd for DreamCast  V 1.0                  #
######################################################################
# including :	Dreamneo cd v1.0 Beta                                #
#  Ported by Ian Micheal Based on   Neogeo cd SDL by fosters         #
#											                         #
#			2004 team and credits									 #
#	#code and co coders quzar black aura troy GPF blue crab		     #
######################################################################
#																	 #
#								   Makefile (c)2020 from Ian Micheal #
######################################################################
*/
//-- Include Files ----------------------------------------------------------- 





#include "neocd.h"



void swab( const void* src1, const void* src2, int isize)
{
	char*	ptr1;
	char*	ptr2;
	char	tmp;
	int	ic1;
	
	ptr1 = (char*)src1;
	ptr2 = (char*)src2;
	for ( ic1=0 ; ic1<isize ; ic1+=2){
		tmp = ptr1[ic1+0];
		ptr2[ic1+0] = ptr1[ic1+1];
		ptr2[ic1+1] = tmp;
	}
}
/*
void	swab1( const void* src1, const void* src2, int isize)
{
	printf("Inside %s line %d\n",__FILE__,__LINE__);
	char*	ptr1;
	char*	ptr2;
	char	tmp;
	int		ic1;
	printf("Inside %s line %d\n",__FILE__,__LINE__);
	ptr1 = (char*)src1;
	ptr2 = (char*)src2;
	for ( ic1=0 ; ic1<isize ; ic1+=2){
		printf("Inside %s line %d\n",__FILE__,__LINE__);
		tmp = ptr1[ic1+0];
		printf("Inside %s line %d\n",__FILE__,__LINE__);
		ptr2[ic1+0] = ptr1[ic1+1];
		printf("Inside %s line %d\n",__FILE__,__LINE__);
		ptr2[ic1+1] = tmp;
		printf("Inside %s line %d\n",__FILE__,__LINE__);
	}
}
*/
