/* Shader taken from
http://www.ozone3d.net/tutorials/vertex_displacement_mapping_p02.php
*/

uniform sampler2D displacementMap;

void main()
{
	vec4 newVertexPos;
	vec4 dv;
	float df;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	dv = texture2D( displacementMap, gl_MultiTexCoord0.xy );
	
	df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
	
	newVertexPos = vec4(gl_Normal * df * 100.0, 0.0) + gl_Vertex;
	
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
}