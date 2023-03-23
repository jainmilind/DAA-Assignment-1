#include <bits/stdc++.h>

using namespace std;
typedef uintptr_t usize;
const int off_set = 200; /*!< This variable stores the offset*/

const int scale = 1; /*!< This is the scale used for the representation */

/**
 * This is the main function and it colours the polygons for better visual distinction and saves it in an images.svg file so that the output can be viewed.
 */
int main()
{
    usize n;
    cin >> n;
    string colors[] = {
        "red", "blue", "green", "yellow", "cyan", "magenta", "gray","orange", "purple", "pink", "brown"};
    string out = "<svg height='210' width='500'>\n";
    for (usize polygon = 1; polygon <= n; polygon++)
    {
        out += "<polygon points='";
        usize vertices;
        cin >> vertices;
        while (vertices--)
        {
            double x, y;
            cin >> x >> y;
            out += to_string(off_set + x * scale) + "," + to_string(off_set + y * -scale) + " ";
        }
        out += "' style='fill:";
        out += colors[(polygon - 1) % (sizeof(colors) / sizeof(colors[0]))];
        out += ";stroke:black;stroke-width:0.1' />\n";
    }
    out += "</svg>";
    freopen("image.svg", "w", stdout);
    cout << out << endl;
}
