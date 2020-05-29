#include "graphicsengine3d.h"
#include <math.h>
#include <iostream>
#include <algorithm>


GraphicsEngine3D::GraphicsEngine3D()
{       
    /*
    meshCube.tris = {

        // SOUTH
        { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        // EAST
        { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

        // NORTH
        { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

        // WEST
        { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

        // TOP
        { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

        // BOTTOM
        { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

    };
    */

    if (!meshCube.LoadFromObjectFile("D:\\Documents\\QT Projects\\3dRaster\\objects\\teapot.obj"))
    {
        std::cout << "Can't open file" << std::endl;
    }

    // Projection Matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)screenHeight() / (float)screenWidth();
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GraphicsEngine3D::renderLater);
    timer->start(10);
}

GraphicsEngine3D::~GraphicsEngine3D()
{

}

void GraphicsEngine3D::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Up:
        inputBuffer.push(Qt::Key_Up);	// Travel Upwards
        break;
    case Qt::Key_Down:
        inputBuffer.push(Qt::Key_Down);	// Travel Down
        break;
    case Qt::Key_Left:
        inputBuffer.push(Qt::Key_Left);	// Travel along x-axis
        break;
    case Qt::Key_Right:
        inputBuffer.push(Qt::Key_Right);	// Travel along x-axis
        break;
    case Qt::Key_W:
        inputBuffer.push(Qt::Key_W);	// Travel Straight
        break;
    case Qt::Key_A:
        inputBuffer.push(Qt::Key_A);	// Turn left
        break;
    case Qt::Key_S:
        inputBuffer.push(Qt::Key_S);	// Travel Back
        break;
    case Qt::Key_D:
        inputBuffer.push(Qt::Key_D);	// Turn right
        break;
    }
}

void GraphicsEngine3D::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerId)
    {
        renderLater();
    }
}

void GraphicsEngine3D::render(QPainter *p, float fElapsedTime)
{
    if (inputBuffer.size() != 0)
    {
        vec3d vForward = Vector_Mul(vLookDir, 8.0f * fElapsedTime);
        switch (inputBuffer.front())
        {
        case Qt::Key_Up:
            vCamera.y += 8.0f * fElapsedTime; // Travel Upwards
            inputBuffer.pop();
            break;
        case Qt::Key_Down:
            vCamera.y -= 8.0f * fElapsedTime;	// Travel Down
            inputBuffer.pop();
            break;
        case Qt::Key_Left:
            vCamera.x -= 8.0f * fElapsedTime;	// Travel along x-axis
            inputBuffer.pop();
            break;
        case Qt::Key_Right:
            vCamera.x += 8.0f * fElapsedTime;	// Travel along x-axis
            inputBuffer.pop();
            break;
        case Qt::Key_W:
            vCamera = Vector_Add(vCamera, vForward);
            inputBuffer.pop();
            break;
        case Qt::Key_A:
            fYaw -= 2.0f * fElapsedTime;
            inputBuffer.pop();
            break;
        case Qt::Key_S:
            vCamera = Vector_Sub(vCamera, vForward);
            inputBuffer.pop();
            break;
        case Qt::Key_D:
            fYaw += 2.0f * fElapsedTime;
            inputBuffer.pop();
            break;
        }
    }

    //Set up "World Transform"
    fTheta += 1.0f * fElapsedTime; // Uncomment to spin me right round baby right round
    mat4x4 matRotZ, matRotX;
    matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
    matRotX = Matrix_MakeRotationX(fTheta);

    mat4x4 matTrans;
    matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

    mat4x4 matWorld;
    matWorld = Matrix_MakeIdentity();	// Form World Matrix
    matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); // Transform by rotation
    matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

    // Create "Point At" Matrix for camera
    vec3d vUp = { 0,1,0 };
    vec3d vTarget = { 0,0,1 };
    mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
    vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
    vTarget = Vector_Add(vCamera, vLookDir);
    mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

    // Make view matrix from camera
    mat4x4 matView = Matrix_QuickInverse(matCamera);

    // Store triagles for rastering later
    std::vector<triangle> vecTrianglesToRaster;

    // Draw Triangles
    for (auto tri : meshCube.tris)
    {
        triangle triProjected, triTransformed, triViewed;

        // World Matrix Transform
        triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
        triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
        triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

        //Calculate triangle normal
        vec3d normal, line1, line2;

        //Get line on either side of triangle
        line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
        line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

        // Take cross product of lines to get normal to triangle surface
        normal = Vector_CrossProduct(line1, line2);

        // You normally need to normalise a normal
        normal = Vector_Normalise(normal);

        // Get Ray from triangle to camera
        vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

        //if ray is aligned with normal then triangle is visible
        if(Vector_DotProduct(normal, vCameraRay) < 0.0f)
        {
            vec3d light_direction = {0.0f, 1.0f, -1.0f};
            float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
            light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

            //how similar is normal to light direction
            float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

            //Choose color as required
            int r = 255 * dp;
            int g = 255 * dp;
            int b = 255 * dp;
            QColor col{r,g,b};
            triTransformed.col = col;

            // Convert World Space into View Space
            triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
            triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
            triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
            triViewed.col = triTransformed.col;

            // Clip Viewed Triangle against near plane, this could form two additional
            // additional triangles.
            int nClippedTriangles = 0;
            triangle clipped[2];
            nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

            // We may end up with multiple triangles form the clip, so project as
            // required
            for (int n = 0; n < nClippedTriangles; n++)
            {
                // Project triangles from 3D --> 2D
                triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
                triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
                triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
                triProjected.col = clipped[n].col;

                // Scale into view, we moved the normalising into cartesian space
                // out of the matrix.vector function from the previous videos, so
                // do this manually
                triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
                triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
                triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

                // X/Y are inverted so put them back
                triProjected.p[0].x *= -1.0f;
                triProjected.p[1].x *= -1.0f;
                triProjected.p[2].x *= -1.0f;
                triProjected.p[0].y *= -1.0f;
                triProjected.p[1].y *= -1.0f;
                triProjected.p[2].y *= -1.0f;

                // Offset verts into visible normalised space
                vec3d vOffsetView = { 1,1,0 };
                triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
                triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
                triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
                triProjected.p[0].x *= 0.5f * (float)screenWidth();
                triProjected.p[0].y *= 0.5f * (float)screenHeight();
                triProjected.p[1].x *= 0.5f * (float)screenWidth();
                triProjected.p[1].y *= 0.5f * (float)screenHeight();
                triProjected.p[2].x *= 0.5f * (float)screenWidth();
                triProjected.p[2].y *= 0.5f * (float)screenHeight();

                // Store triangle for sorting
                vecTrianglesToRaster.push_back(triProjected);
            }
        }
    }
    // Sort triangles from back to front
    sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
    {
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
    });

    // Loop through all transformed, viewed, projected, and sorted triangles
    for (auto &triToRaster : vecTrianglesToRaster)
    {
        // Clip triangles against all four screen edges, this could yield
        // a bunch of triangles, so create a queue that we traverse to
        //  ensure we only test new triangles generated against planes
        triangle clipped[2];
        std::list<triangle> listTriangles;

        // Add initial triangle
        listTriangles.push_back(triToRaster);
        int nNewTriangles = 1;

        for (int p = 0; p < 4; p++)
        {
            int nTrisToAdd = 0;
            while (nNewTriangles > 0)
            {
                // Take triangle from front of queue
                triangle test = listTriangles.front();
                listTriangles.pop_front();
                nNewTriangles--;

                // Clip it against a plane. We only need to test each
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                switch (p)
                {
                case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)screenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)screenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++)
                    listTriangles.push_back(clipped[w]);
            }
            nNewTriangles = listTriangles.size();
        }


        // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
        for (auto &t : listTriangles)
        {
            fillTriangle(p, t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.col);
            //DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_SOLID, FG_BLACK);
        }
    }

}
