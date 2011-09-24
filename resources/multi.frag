uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
	vec4 texel0, texel1;
	texel0 = texture2D(tex0,gl_TexCoord[0].st);
	texel1 = texture2D(tex1,gl_TexCoord[0].st);
	
	gl_FragData[0] = texel0;
	gl_FragData[1] = texel1;
}