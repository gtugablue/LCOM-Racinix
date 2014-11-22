Sprite *create_sprite(char *pic[], char *bas)
{
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}
	return sp;
}

int animate_sprite(Sprite *sp, char *base)
{
	// TODO
}

void destroy_sprite(Sprite *sp, char *base)
{
	if( sp == NULL )
		return;
	free(sp->map);
	free(sp);
}
