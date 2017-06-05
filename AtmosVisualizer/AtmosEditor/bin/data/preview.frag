#version 430

uniform sampler2D tex;

//in vec4 colorVarying;

out vec4 fragColor;
// uniform vec4 globalColor;

// out vec4 fragColor;

void main (void)
{
	//gl_FragColor = gl_Color;  
	//fragColor = globalColor;
	//fragColor = texture(tex, gl_PointCoord);
	fragColor = vec4(1, 0, 0, 1);
}