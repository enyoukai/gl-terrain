#version 330

out vec4 fragColor;

in float height;
in vec3 Normal;

vec3 lightColor = vec3(1.0, 1.0, 1.0);
float ambientStrength = 0.1;

// MAKE THESE ENUMS AND UNIFORMS LATER 
void main()
{
	vec3 ambient = ambientStrength * lightColor;

	vec3 objectColor;
	if (height > 45)
	{
		objectColor = vec3(1.0f, 1.0f, 1.0f);
	}
	else if (height > -10)
	{
		objectColor = vec3(91.0f/255.0f, 91.0f/255.0f, 91.0f/255.0f);
	}
	else if (height > -30)
	{
		// fragColor = vec4(0.34f, 0.49f, 0.27f, 1.0f);
		objectColor = vec3(0.76f, 0.70f, 0.50f);
		
	}
	else if (height > -40)
	{
		objectColor = vec3(0.76f, 0.70f, 0.50f);
	}
	else
	{
		objectColor = vec3(14.0/255.0, 131.0/255.0, 198.0 / 255.0);
	}

	vec3 finalColor = ambient * objectColor;

	fragColor = vec4(finalColor, 1.0);
}