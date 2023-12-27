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
        unsigned char* row = teximage + (tex_width * (tex_height - 1));
        for( int y = 0; y < tex_height; ++y )
        {
            fwrite( row, tex_width, 1, fp );
            row -= tex_width;
        }
#else
        fwrite( teximage, tex_width * tex_height, 1, fp );
#endif
    }

    fclose( fp );
}
