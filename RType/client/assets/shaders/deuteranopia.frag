#version 330

uniform sampler2D texture0;
uniform vec4 colDiffuse;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord) * fragColor * colDiffuse;
    float r = color.r;
    float g = color.g;
    float b = color.b;

    // Deuteranopia simulation
    float newR = (0.625 * r) + (0.375 * g) + (0.0 * b);
    float newG = (0.7 * r) + (0.3 * g) + (0.0 * b);
    float newB = (0.0 * r) + (0.3 * g) + (0.7 * b);

    finalColor = vec4(newR, newG, newB, color.a);
}
