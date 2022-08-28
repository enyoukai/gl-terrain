#version 330

out vec4 fragColor;
in float height;

// MAKE THESE ENUMS AND UNIFORMS LATER 
void main()
{
	if (height > 45)
	{
		fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (height > -10)
	{
		fragColor = vec4(91.0f/255.0f, 91.0f/255.0f, 91.0f/255.0f, 1.0f);
	}
	else if (height > -30)
	{
		// fragColor = vec4(0.34f, 0.49f, 0.27f, 1.0f);
		fragColor = vec4(0.76f, 0.70f, 0.50f, 1.0f);

		
	}
	else if (height > -40)
	{
		fragColor = vec4(0.76f, 0.70f, 0.50f, 1.0f);
	}
	else
	{
		fragColor = vec4(14.0/255.0, 131.0/255.0, 198.0 / 255.0, 1.0f);
	}
}