#version 330

out vec4 fragColor;

in float height;
in vec3 Normal;
in vec3 Position;

vec3 lightColor = vec3(1.0, 1.0, 1.0);
float ambientStrength = 0.1;

uniform vec3 lightPosition;
uniform bool showNormals;

float constant = 1.0;
float linear = 0.014;
float quadratic = 0.0007;

// MAKE THESE ENUMS AND UNIFORMS LATER 
void main()
{
	vec3 objectColor;

	vec3 ambient = ambientStrength * lightColor;

	if (height > 45)
	{
		objectColor = vec3(1.0f, 1.0f, 1.0f);
	}
	else if (height > 20)
	{
		objectColor = vec3(91.0f/255.0f, 91.0f/255.0f, 91.0f/255.0f);
	}
	else if (height > 10)
	{
		// fragColor = vec4(0.34f, 0.49f, 0.27f, 1.0f);
		objectColor = vec3(0.76f, 0.70f, 0.50f);
		
	}
	else if (height > 0)
	{
		objectColor = vec3(0.76f, 0.70f, 0.50f);
	}
	else
	{
		objectColor = vec3(14.0/255.0, 131.0/255.0, 198.0 / 255.0);
	}

	vec3 lightRay = normalize(lightPosition - Position);
	float diffuseStrength = max(dot(lightRay, Normal), 0.0);

	vec3 diffuse = diffuseStrength * lightColor;

// TEST
	float distance = length(lightPosition - Position);
	float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));   
	ambient *= attenuation;
	diffuse *= attenuation;
// TEST
	vec3 finalColor = (ambient + diffuse) * objectColor;

	if (showNormals)
	{
		fragColor = vec4(Normal, 1.0);
	}
	else
	{
		fragColor = vec4(finalColor, 1.0);

	}


}