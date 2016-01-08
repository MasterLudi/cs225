/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>

#include "maptiles.h"


/**
 * Map the image tiles into a mosaic canvas which closely
 * matches the input image.
 */
mosaic_canvas map_tiles(const source_image& source,
                        const std::vector<tile_image>& tiles)
{

	// make a empty mosaic canvas wt same rows, cols of source
	mosaic_canvas canvas{source.rows(), source.columns()};
	
	// make 3d tree of tiles
	std::vector<point<3>> points;

	// key = average color converted to points, val = tile image
	std::map<point<3>, tile_image> tile_dic;

	for (int i=0; i<tiles.size(); i++) {
		// convert average to point and store
		points.push_back(tile_image_to_pt(tiles[i]));
		tile_dic[points[i]] = tiles[i];
	}

	// build the kd_tree
	kd_tree<3> avg_tree{points};
	
	for (int i=0; i<source.rows(); i++) {
		for (int j=0; j<source.columns(); j++) {
			// convert region_color to a point
			point<3> source_avg = rgba_to_pt(source.region_color(i, j));
			point<3> nearest = avg_tree.find_nearest_neighbor(source_avg);
			canvas.set_tile(i, j, tile_dic[nearest]);
		}
	}
	
	return canvas;

}

point<3> tile_image_to_pt(const tile_image& tile)
{
	epng::rgba_pixel tmp = tile.average_color();
	point<3> ret(tmp.red, tmp.green, tmp.blue);
	return ret;
}

point<3> rgba_to_pt(const epng::rgba_pixel& pix)
{
	point<3> ret{pix.red, pix.green, pix.blue};
	return ret;
}
