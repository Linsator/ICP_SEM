#version 330 core

in vec2 texcoord;
in vec3 Normal;
in vec3 crntPos;
in mat4 modelM;

uniform vec4 u_diffuse_color;

// Gets the position of the camera from the main function
uniform vec3 camPos;

uniform sampler2D tex0; // texture unit from C++


struct Light {
	// Gets the color of the light from the main function
	vec4 lightColor;
	// Gets the position of the light from the main function
	vec3 lightPos;
	float intenA;
	float intenB;
	float ambient;
};

uniform Light lights[4];

out vec4 FragColor; // final output


vec4 pointLight(Light light)
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = vec3(modelM * vec4(light.lightPos, 1.0f)) - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float inten = 1000.0f / (light.intenA * dist * dist + light.intenB * dist + 1.0f);

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(tex0, texcoord) * (diffuse * inten + light.ambient + specular * inten)) * light.lightColor;
}

vec4 direcLight(Light light)
{
	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(tex0, texcoord) * (diffuse + light.ambient))* light.lightColor;
}

vec4 spotLight(Light light)
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(modelM * vec4(light.lightPos, 1.0f)) - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(tex0, texcoord) * (diffuse * inten + light.ambient +  specular * inten)) * light.lightColor;
}


void main()
{
	vec4 directLight = direcLight(lights[0]);
	vec4 pointLight1 = pointLight(lights[1]);
	vec4 pointLight2 = pointLight(lights[2]);
	vec4 pointLight3 = pointLight(lights[3]);

    FragColor = directLight + pointLight1 + pointLight2 + pointLight3;

}