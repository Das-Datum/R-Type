#version 330

uniform sampler2D texture0;
uniform vec4 colDiffuse;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord) * fragColor * colDiffuse;
    float gray = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;

    finalColor = vec4(gray, gray, gray, color.a);
}
