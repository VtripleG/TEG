#include "TEG.h"
#include <iostream>
#include <memory>

int main( int argc, char** argv )
{
  setlocale( LC_ALL, "" );
  TEG tegWin;

  if ( argc > 1 )
    tegWin = TEG( argv[1] );

  tegWin.Start();

  return 0;
}
