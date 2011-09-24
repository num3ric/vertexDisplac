uniform sampler2D colorMap;

void main(void)
{
   gl_FragData[0] = texture2D(colorMap, gl_TexCoord[0].st);
}