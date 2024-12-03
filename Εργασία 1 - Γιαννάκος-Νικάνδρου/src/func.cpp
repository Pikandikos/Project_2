#include "./func.h"
#include <iostream>

bool add_steiner_point_local_search(CDT &cdt, const CDT::Edge &edge, const vector<pair<Point_2, Point_2>> &constraints)
{
    vector<Point_2> contender;

    // Check if the edge is valid
    if (!edge.first->is_valid())
    {
        cerr << "Invalid edge detected, skipping Steiner point insertion" << endl;
        return false;
    }

    CDT::Vertex_handle vh1 = edge.first->vertex((edge.second + 1) % 3);
    CDT::Vertex_handle vh2 = edge.first->vertex((edge.second + 2) % 3);

    // Validate the face handle and vertices
    if (!edge.first->is_valid() || vh1 == nullptr || vh2 == nullptr)
    {
        cerr << "Invalid edge or vertices detected, skipping Steiner point insertion" << endl;
        return false; // Early exit
    }

    // Check for degeneracy before circumcenter calculation (εκφυλισμένη κορυφή)
    if (CGAL::collinear(vh1->point(), vh2->point(), edge.first->vertex(edge.second)->point()))
    {
        cerr << "Degenerate triangle detected, skipping circumcenter calculation" << endl;
        return false; // Early exit to avoid inserting into an invalid edge
    }

    // Attempt to use the circumcenter as the Steiner point
    Point_2 circumcenter = CGAL::circumcenter(vh1->point(), vh2->point(), edge.first->vertex(edge.second)->point());
    contender.push_back(circumcenter);
    // Midpoint of the longest Edge
    Point_2 midpoint = CGAL::midpoint(vh1->point(), vh2->point());
    contender.push_back(midpoint);
    // Projection of the obtuse vertex onto the opposite edge
    Point_2 projection = CGAL::projection(CGAL::Segment_2(vh1->point(), vh2->point()), obtuse_vertex);
    contender.push_back(projection);
    // Centroid of the triangle
    Point_2 centroid = CGAL::centroid(vh1->point(), vh2->point(), obtuse_vertex);
    contender.push_back(centroid);
    // Mean point of adjacent obtuse triangles
    Point_2 mean_point_of_adjacent_triangles = mean_point_of_adjacent_triangles(cdt, edge.first, constraints);
    contender.push_back(mean_point_of_adjacent_triangles);

    for (int i = 0; i < 5; i++)
    {
        bool not_inside = false;
        bool already_exists = false;

        // Validate if the point is within constraints
        if (!is_point_inside_constraints(contender[i], constraints))
        {
            std::cerr << "Point is outside the constraints." << endl;
            not_inside = false;
        }

        // Check if the point already exists as a vertex
        for (auto vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); ++vit)
        {
            if (vit->point() == contender[i])
            {
                std::cerr << "Point is already exists in CDT." << endl;
                already_exists = true;
                break;
            }
        }
    }

    std::cerr << "No Steiner points found that were valid, skipping insertion." << std::endl;
    return false;
}

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

CDT triangulation(vector<Point_2> &points, vector<int> &region_boundary, const vector<pair<int, int>> &additional_constraints)
{
    // τριγωνοποίηση Delaunay
    CDT cdt;

    // Manually store the constraints as pairs of points
    vector<std::pair<Point_2, Point_2>> constraints;

    // προσθήκη περιορισμένων ακμών (PSLG)
    // Ensure region boundary constraints are added in CCW order
    for (std::size_t i = 0; i < region_boundary.size() - 1; ++i)
    {
        Point_2 p1 = points[region_boundary[i]];
        Point_2 p2 = points[region_boundary[i + 1]];
        cdt.insert_constraint(p1, p2);
        constraints.push_back({p1, p2}); // Store the region boundary constraint
    }

    // Close the loop for region_boundary if needed
    if (region_boundary.size() > 2)
    {
        Point_2 p1 = points[region_boundary.back()];
        Point_2 p2 = points[region_boundary.front()];
        cdt.insert_constraint(p1, p2);
        constraints.push_back({p1, p2}); // Store the closing constraint
    }

    // Add additional constraints
    for (const auto &constraint : additional_constraints)
    {
        Point_2 p1 = points[constraint.first];
        Point_2 p2 = points[constraint.second];
        cdt.insert_constraint(p1, p2);
        constraints.push_back({p1, p2}); // Store the additional constraint
    }

    check_cdt_validity(cdt);

    // προσθήκη σημείων από τον vector points με έλεγχο των constraints
    for (const auto &point : points)
    {
        // Check if the point is inside the constraints
        if (is_point_inside_constraints(point, constraints))
        {
            cdt.insert(point); // εισαγωγή σημείου στην τριγωνοποίηση
            check_cdt_validity(cdt);
        }
        else
        {
            std::cerr << "Point (" << point.x() << ", " << point.y() << ") is outside constraints and will be skipped.\n";
        }
    }

    // επανάληψη για προσθήκη σημείων Steiner αν υπάρχουν αμβλυγώνια τρίγωνα
    bool all_acute = false;
    bool steiner_point_inserted;
    bool steiner_point_added_this_rotation;
    int no_of_steiner_points_added = 0;

    while (!all_acute && no_of_steiner_points_added < MAX_NO__STEINER_POINTS)
    {
        // Visualize the triangulation
        // export_to_svg(cdt, "output.svg");
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
                steiner_point_inserted = add_steiner_point_local_search(cdt, CDT::Edge(face_it, 0), constraints);
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
                steiner_point_inserted = add_steiner_point_local_search(cdt, CDT::Edge(face_it, 2), constraints);
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
                steiner_point_inserted = add_steiner_point_local_search(cdt, CDT::Edge(face_it, 1), constraints);
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