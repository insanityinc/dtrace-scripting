provider tracepcp
{
  probe query__countnonzeros( int );
  probe query__checksolution( int );
  probe query__multiplication( int, int, int );
  probe query__computemultiplicationtime( double );
};
