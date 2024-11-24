#include "./func.h"
#include <iostream>

#include <cmath>

// Define a tolerance for comparing points
const double epsilon = 1e-6;

bool are_points_equal(const Point_2 &p1, const Point_2 &p2)
{
    return std::fabs(p1.x() - p2.x()) < epsilon && std::fabs(p1.y() - p2.y()) < epsilon;
}

bool add_steiner_point_on_edge(CDT &cdt, const CDT::Edge &edge)
{
    // Check if the edge is valid
    if (!edge.first->is_valid())
    {
        cerr << "Invalid edge detected, skipping Steiner point insertion" << endl;
        return false;
    }

    CDT::Vertex_handle vh1 = edge.first->vertex((edge.second + 1) % 3);
    CDT::Vertex_handle vh2 = edge.first->vertex((edge.second + 2) % 3);

    // Circumcenter calculation
    Point_2 circumcenter = CGAL::circumcenter(vh1->point(), vh2->point());

    // Check if circumcenter exists by comparing it to existing vertices in the triangulation
    bool circumcenter_exists = false;
    for (auto v = cdt.vertices_begin(); v != cdt.vertices_end(); ++v)
    {
        if (are_points_equal(v->point(), circumcenter))
        {
            circumcenter_exists = true;
            break;
        }
    }

    if (circumcenter_exists)
    {
        cerr << "Circumcenter already exists, skipping insertion." << endl;
    }
    else
    {
        CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);
        if (new_vertex != nullptr)
        {
            cout << "Steiner Point (circumcenter) added: (" << circumcenter.x() << ", " << circumcenter.y() << ")" << endl;
            return true;
        }
    }

    // Midpoint calculation
    Point_2 midpoint = CGAL::midpoint(vh1->point(), vh2->point());

    // Check if midpoint exists by comparing it to existing vertices in the triangulation
    bool midpoint_exists = false;
    for (auto v = cdt.vertices_begin(); v != cdt.vertices_end(); ++v)
    {
        if (are_points_equal(v->point(), midpoint))
        {
            midpoint_exists = true;
            break;
        }
    }

    if (midpoint_exists)
    {
        cerr << "Midpoint already exists, skipping insertion." << endl;
    }
    else
    {
        CDT::Vertex_handle new_vertex = cdt.insert(midpoint);
        if (new_vertex != nullptr)
        {
            cout << "Steiner Point (midpoint) added: (" << midpoint.x() << ", " << midpoint.y() << ")" << endl;
            return true;
        }
    }

    std::cerr << "Both circumcenter and midpoint Steiner points already exist, skipping insertion." << std::endl;
    return false;
}

// bool add_steiner_point_on_edge(CDT &cdt, const CDT::Edge &edge)
// {
//     // Check if the edge is valid
//     if (!edge.first->is_valid())
//     {
//         cerr << "Invalid edge detected, skipping Steiner point insertion" << endl;
//         return false;
//     }

//     CDT::Vertex_handle vh1 = edge.first->vertex((edge.second + 1) % 3);
//     CDT::Vertex_handle vh2 = edge.first->vertex((edge.second + 2) % 3);

//     // // Assuming edge.first is a valid face handle and you want to print its vertices
//     // cout << "Edge first (face vertices): ";
//     // for (int i = 0; i < 3; ++i)
//     // {
//     //     auto vertex_handle = edge.first->vertex(i); // Get the vertex handle
//     //     if (vertex_handle != nullptr)
//     //     {
//     //         cout << "(" << vertex_handle->point().x() << ", " << vertex_handle->point().y() << ") ";
//     //     }
//     // }
//     // cout << ", Edge second index: " << edge.second << endl;

//     // if (vh1 != nullptr && vh2 != nullptr)
//     // {
//     //     cout << "Vertex handle 1: (" << vh1->point().x() << ", " << vh1->point().y() << "), "
//     //          << "Vertex handle 2: (" << vh2->point().x() << ", " << vh2->point().y() << ")" << endl;
//     // }
//     // else
//     // {
//     //     cout << "One or both vertex handles are null." << endl;
//     // }

//     // // Check if the vertex handles are valid
//     // if (vh1 == NULL || vh2 == NULL)
//     // {
//     //     cerr << "Invalid vertex handle detected, skipping Steiner point insertion" << endl;
//     //     return false; // Early exit to avoid inserting into an invalid edge
//     // }

//     // Check for degeneracy before circumcenter calculation (εκφυλισμένη κορυφή)
//     if (CGAL::collinear(vh1->point(), vh2->point(), edge.first->vertex(edge.second)->point()))
//     {
//         cerr << "Degenerate triangle detected, skipping circumcenter calculation" << endl;
//         return false; // Early exit to avoid inserting into an invalid edge
//     }

//     // Find the Circumcenter
//     Point_2 circumcenter = CGAL::circumcenter(vh1->point(), vh2->point());

//     // Check if circumcenter exists
//     if (cdt.locate(circumcenter) != nullptr)
//     {
//         cerr << "Circumcenter already exists, skipping insertion." << endl;
//         cerr << "Skipping insertion for edge with vertices: ("
//              << vh1->point() << ") and (" << vh2->point() << ") ";
//         cerr << "Triangle vertices: (" << edge.first->vertex(0)->point() << "), "
//              << "(" << edge.first->vertex(1)->point() << "), "
//              << "(" << edge.first->vertex(2)->point() << ")" << endl;
//     }
//     else
//     {
//         CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);
//         if (new_vertex != nullptr)
//         {
//             cout << "Steiner Point (circumcenter) added: (" << circumcenter.x() << ", " << circumcenter.y() << ")" << endl;
//             return true;
//         }
//     }

//     // Find the Midpoint
//     Point_2 midpoint = CGAL::midpoint(vh1->point(), vh2->point());

//     // Check if midpoint exists
//     if (cdt.locate(midpoint) != nullptr)
//     {
//         cerr << "Midpoint already exists, skipping insertion." << endl;
//         cerr << "Skipping insertion for edge with vertices: ("
//              << vh1->point() << ") and (" << vh2->point() << ") ";
//         cerr << "Triangle vertices: (" << edge.first->vertex(0)->point() << "), "
//              << "(" << edge.first->vertex(1)->point() << "), "
//              << "(" << edge.first->vertex(2)->point() << ")" << endl;
//     }
//     else
//     {
//         CDT::Vertex_handle new_vertex = cdt.insert(midpoint);
//         if (new_vertex != nullptr)
//         {
//             cout << "Steiner Point (midpoint) added: (" << midpoint.x() << ", " << midpoint.y() << ")" << endl;
//             return true;
//         }
//     }

//     std::cerr << "Both circumcenter and midpoint Steiner points already exist, skipping insertion." << std::endl;
//     return false;
// }

bool attempt_to_flip(CDT &cdt, CDT::Finite_faces_iterator face_it, CDT::Edge edge)
{
    CDT copied_cdt = cdt; // Create a copy of the CDT

    // Ensure the edge has two distinct faces
    CDT::Face_handle face0 = edge.first;
    CDT::Face_handle face1 = face0->neighbor(edge.second);
    if (face1 == nullptr || face0 == nullptr || face0 == face1)
    {
        std::cerr << "Error: Edge does not have two distinct valid faces for flipping... Extiting attempt_to_flip" << std::endl;
        return false;
    }

    if (cdt.is_infinite(face0) || cdt.is_infinite(face1))
    {
        std::cerr << "One of the faces is infinite." << std::endl;
        return false; // Handle this case appropriately
    }

    // Perform the edge flip in the copied CDT
    copied_cdt.flip(face0, edge.second);
    check_cdt_validity(cdt);

    // Check angles in the copied CDT
    bool all_acute = true;
    for (CDT::Finite_faces_iterator fit = copied_cdt.finite_faces_begin(); fit != copied_cdt.finite_faces_end(); ++fit)
    {
        Point_2 p1 = fit->vertex(0)->point();
        Point_2 p2 = fit->vertex(1)->point();
        Point_2 p3 = fit->vertex(2)->point();

        double angle1 = angle_between_points(p1, p2, p3);
        double angle2 = angle_between_points(p2, p1, p3);
        double angle3 = angle_between_points(p3, p1, p2);

        if (angle1 > 90 || angle2 > 90 || angle3 > 90)
        {
            all_acute = false;
            break;
        }
    }

    if (all_acute)
    {
        cout << "Flipping Edge!!!" << endl;
        cdt = copied_cdt; // Apply the flip if all angles are acute
        return true;
    }
    else
    {
        std::cerr << "Flip resulted in non-acute angles, reverting changes." << std::endl;
        return false; // Revert to the original CDT
    }
}

CDT triangulation(vector<Point_2> &points, vector<int> &region_boundary)
{
    // τριγωνοποίηση Delaunay
    CDT cdt;

    // προσθήκη σημείων από τον vector points
    for (const auto &point : points)
    {
        cdt.insert(point); // εισαγωγή σημείου στην τριγωνοποίηση
        check_cdt_validity(cdt);
    }

    // προσθήκη περιορισμένων ακμών (PSLG)
    for (std::size_t i = 0; i < region_boundary.size() - 1; i++ /*i += 2*/)
    { // ζεύγος δεικτών αναπαριστουν ακμή, ορια περιοχης που θα τριγωνοποιηθει
        cdt.insert_constraint(points[region_boundary[i]], points[region_boundary[i + 1]]);
    }

    check_cdt_validity(cdt);

    // επανάληψη για προσθήκη σημείων Steiner αν υπάρχουν αμβλυγώνια τρίγωνα
    bool all_acute = false;
    bool steiner_point_inserted;
    bool steiner_point_added_this_rotation;
    int no_of_steiner_points_added = 0;

    while (!all_acute)
    {
        // Visualize the triangulation
        export_to_svg(cdt, "output.svg");
        all_acute = true; // υποθετω ολα τα τριγωνα οξυγωνια
        cout << endl
             << "Starting to check angles... again" << endl;

        int face_count = cdt.number_of_faces();
        cout << "Number of faces: " << face_count << endl;

        for (CDT::Finite_faces_iterator face_it = cdt.finite_faces_begin(); face_it != cdt.finite_faces_end(); face_it++)
        {
            steiner_point_added_this_rotation = false;
            if (!face_it->is_valid())
            {
                std::cerr << "Invalid face detected, skipping." << endl;
                continue; // Skip invalid faces
            }

            Point_2 p1 = face_it->vertex(0)->point();
            Point_2 p2 = face_it->vertex(1)->point();
            Point_2 p3 = face_it->vertex(2)->point();

            cout << "P1:" << p1 << "  P2:" << p2 << "  P3:" << p3 << endl;

            // Υπολογισμός των γωνιών του τριγώνου
            double angle1 = angle_between_points(p1, p2, p3);
            double angle2 = angle_between_points(p2, p1, p3);
            double angle3 = angle_between_points(p3, p1, p2);

            cout << "Angle1:" << angle1 << "  Angle2:" << angle2 << "  Angle3:" << angle3 << endl;

            if (angle1 == 0 || angle2 == 0 || angle3 == 0)
            {
                cout << "Angle = 0 Found !!!!" << endl;
                continue;
            }

            // if (angle1 > 90 || angle2 > 90 || angle3 > 90)
            // {
            //     all_acute = false;
            //     bool flipped = false;

            //     // Try to flip the edges in the order of angles, fallback to Steiner point if flipping fails
            //     if (angle1 > 90)
            //         flipped = attempt_to_flip(cdt, face_it, CDT::Edge(face_it, 2));

            //     if (!flipped && angle2 > 90)
            //         flipped = attempt_to_flip(cdt, face_it, CDT::Edge(face_it, 0));

            //     if (!flipped && angle3 > 90)
            //         flipped = attempt_to_flip(cdt, face_it, CDT::Edge(face_it, 1));

            //     if (!flipped) // If flipping fails, add a Steiner point
            //     {

            if (angle1 > 90)
            {
                all_acute = false;
                bool flipped = false;
                steiner_point_inserted = add_steiner_point_on_edge(cdt, CDT::Edge(face_it, 2));
                if (steiner_point_inserted) // steiner point hasn't been skipped
                {
                    no_of_steiner_points_added++;
                    cout << "No. of Steiner Points: " << no_of_steiner_points_added << endl;
                    face_it = cdt.finite_faces_begin();
                    steiner_point_added_this_rotation = true;
                    break; // Steiner Point has been added... Now Restart the checking of angles
                }
            }
            else if (angle2 > 90)
            {
                all_acute = false;
                bool flipped = false;
                steiner_point_inserted = add_steiner_point_on_edge(cdt, CDT::Edge(face_it, 0));
                if (steiner_point_inserted) // steiner point hasn't been skipped
                {
                    no_of_steiner_points_added++;
                    cout << "No. of Steiner Points: " << no_of_steiner_points_added << endl;
                    face_it = cdt.finite_faces_begin();
                    steiner_point_added_this_rotation = true;
                    break;
                }
            }
            else if (angle3 > 90)
            {
                all_acute = false;
                bool flipped = false;
                steiner_point_inserted = add_steiner_point_on_edge(cdt, CDT::Edge(face_it, 1));
                if (steiner_point_inserted) // steiner point hasn't been skipped
                {
                    no_of_steiner_points_added++;
                    cout << "No. of Steiner Points: " << no_of_steiner_points_added << endl;
                    face_it = cdt.finite_faces_begin();
                    steiner_point_added_this_rotation = true;
                    break;
                }
            }
            // }
            // }
        }
        if (steiner_point_added_this_rotation == false) // Will need to add flip if it was working
        {
            cout << "All faces/triangles are acute" << endl;
            all_acute = true; // If the loop ever ends it means that all faces are acute
        }
    }

    return cdt;
}