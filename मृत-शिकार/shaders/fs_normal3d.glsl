#version 330

in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fShadowCoord;

layout (location = 0) out vec4 fColor;

uniform sampler2D texture_sample;
uniform sampler2DShadow texture_depthmap;


vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

void main()
{
	vec4 fTexColor = texture2D(texture_sample, fTexCoord);

	vec3 dir = normalize(vec3(-1.0, -1.0, 0.0));
    float fDiffuseIntensity = max(0, dot(normalize(fNormal), - dir ));
	
	float visibility = 1.0;
	// the loop takes care of neighbouring samples as well
	// larger spread means much nearer samples
	float spread = 1200.0f;
	for (int i = 0; i<7; i++){ //i<4
		int index = i;
		//0.2 *
		visibility -= 0.08*(1.0 - texture(texture_depthmap,
			vec3((fShadowCoord.xy / fShadowCoord.w) + poissonDisk[index] / spread,
			(fShadowCoord.z) / fShadowCoord.w)));
	}

    fColor = vec4(vec3(0.45+fDiffuseIntensity*visibility), 1.0) * fTexColor;
}
