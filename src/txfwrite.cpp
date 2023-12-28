#include "txfwrite.h"


TexFontWriter::~TexFontWriter()
{
    delete[] tgi;
}


void TexFontWriter::setGlyphCount( int n )
{
    num_glyphs = n;
    delete[] tgi;
    tgi = new TexGlyphInfo[ n ];
}


void TexFontWriter::write( const char* filename )
{
    FILE* fp;
    int endianness;

    assert( sizeof(int) == 4 );  /* Ensure external file format size. */

    fp = fopen( filename, "wb" );
    if( ! fp )
    {
        fprintf( stderr, "Failed to open %s\n", filename );
        return;
    }

    endianness = 0x12345678;

    fwrite( "\377txf", 1, 4, fp );
    fwrite( &endianness,  sizeof(int), 1, fp );
    fwrite( &format,      sizeof(int), 1, fp );
    fwrite( &tex_width,   sizeof(int), 1, fp );
    fwrite( &tex_height,  sizeof(int), 1, fp );
    fwrite( &max_ascent,  sizeof(int), 1, fp );
    fwrite( &max_descent, sizeof(int), 1, fp );
    fwrite( &num_glyphs,  sizeof(int), 1, fp );
    
    if ( g_verbose ) 
    {
        printf("num_glyphs = %d\n", num_glyphs);
    }

    for( int i = 0; i < num_glyphs; ++i )
    {
        tgi[ i ].dummy = 0;
        fwrite( &tgi[ i ], sizeof(TexGlyphInfo), 1, fp );
    }

    if( format == TXF_FORMAT_BITMAP )
    {
        fprintf( stderr, "TXF_FORMAT_BITMAP not handled\n" );
    }
    else
    {
#if 1
        unsigned char* row = tex_image + (tex_width * (tex_height - 1));
        for( int y = 0; y < tex_height; ++y )
        {
            fwrite( row, tex_width, 1, fp );
            row -= tex_width;
        }
#else
        fwrite( tex_image, tex_width * tex_height, 1, fp );
#endif
    }

    fclose( fp );
}


#ifdef DEBUG

void TexFontWriter::dump( int pitch, bool crop )
{
    int x, y, w, h;    
	unsigned char* buf = tex_image;
	
	w = tex_width;
	h = tex_height;

	printf( "txf dump:  pitch=%d  w=%d h=%d crop=%s\n", pitch, w, h, bool_to_str( crop ) );

    /* Fit on 80 column terminal. */
    if( crop && w > 39 )
        w = 39;

    for( y = 0; y < h; y++ )
    {
        for( x = 0; x < w; x++ )
            printf( "%02x", (int) *(buf + x) );
        printf( "\n" );
        buf += pitch;
    }
    printf( "\n" );
}

#endif
