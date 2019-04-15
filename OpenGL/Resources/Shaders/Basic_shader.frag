#version 330 core

in vec2 TexCoord;
in vec3 Normal; 
in vec3 Vertex; 
//Texture sampler
uniform sampler2D texture_diffuse1;
//Light information
uniform vec4 lightAmbient;
uniform vec3 lightAttenuation;
uniform vec4		matDiffuse;
uniform vec4		lamberTerm;
uniform vec3		eyePos;
uniform float		matSpecularExponent;
uniform vec4		matSpecularColour;
uniform vec4		specularIntensity;
//Light uniforms
//uniform LightProperties light1, light3, light5, light7;
//Material iformation
uniform vec4		matAmbient;
out vec4 FragColour;
//Function prototype
vec4 calculateLight(vec4 lightPosition, vec4 lightDiffuse);

void main(){
	vec4 light1 = calculateLight(vec4(-10.0,5.0,-4.0, 1.0), vec4 (1.0,5.0,1.0,2.0));
	vec4 light3 = calculateLight(vec4(-4.0, 5.0, -4.0, 1.0), vec4 (1.0,1.0,5.0,2.0));
	vec4 light5 = calculateLight(vec4(2.0, 5.0, -4.0, 1.0), vec4 (0.0,0.0,0.0,2.0));
	vec4 light7 = calculateLight(vec4(8.0, 5.0, -4.0, 1.0), vec4 (2.5,2.5,2.5,2.0));

	FragColour = light1 + light3 + light5 + light7;
}

vec4 calculateLight(vec4 lightPosition, vec4 lightDiffuse ){
	//Attenuation/drop-off	
	float d = length(lightPosition.xyz - Vertex);
	float att = 1.0 / (lightAttenuation.x + lightAttenuation.y + lightAttenuation.z * (d * d));
	
	//Ambient light value
	vec4 texColour = texture(texture_diffuse1, TexCoord);
	vec4 ambient = lightAmbient * matAmbient * texColour * att;

	//Diffuse light value
	vec3 N = normalize(Normal);
	vec3 L = normalize(lightPosition.xyz - Vertex);
	float lamberTerm = clamp(dot(N, L), 0.0, 1.0);
	vec4 diffuse = lightDiffuse * matDiffuse * lamberTerm * texColour * att;


	//Specular light value
	vec3 E = normalize(eyePos - Vertex);
	vec3 R = reflect(-L, N); // reflected light vector about normal N
	float specularIntensity = pow(max(dot(E,R), 0.0), matSpecularExponent);
	vec4 specular = matSpecularColour * specularIntensity * texColour * att;

	return 	ambient + diffuse + specular;
}