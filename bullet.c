#include "bullet.h"

Bullet *bullets_list;

Bullet *Bullet_Create(Coordinate _location, SpriteGraphic _image, Coordinate _deltaPerFrame, bool _isPlayer)
{
  Bullet *bullet = calloc(1, sizeof(Bullet));

  bullet->next = NULL;
  bullet->location = (Coordinate){ .x = _location.x, .y = _location.y };
  bullet->image = (SpriteGraphic){ .location = _image.location, .size = _image.size };
  bullet->deltaPerFrame = (Coordinate){ .x = _deltaPerFrame.x, .y = _deltaPerFrame.y };
  bullet->isPlayer = _isPlayer;

  return bullet;
}

void Bullets_Insert(Bullet **head, Bullet *new)
{
  new->next = *head;
  *head = new;
}

void Bullets_Update(Bullet **head)
{
  Bullet *current = NULL;
  Bullet *previous = NULL;

  if(*head != NULL){
    //skunkCONSOLEWRITE("*** start bullet list ***\n");

    for(current = *head; current != NULL; previous = current, current = current->next)
      {	
	//	sprintf(skunkoutput, "bullet: addr %08p, location (%03d,%03d)\n", current, current->location.x, current->location.y);
	//skunkCONSOLEWRITE(skunkoutput);

       	current->location.x += current->deltaPerFrame.x;
	current->location.y += current->deltaPerFrame.y;
	if(current->location.y < 5)
	  {
	    if(previous != NULL)
	      {
		//If there's a previous node, set its next ptr to current->next
		previous->next = current->next;
	      }
	    else 
	      {
		//If there is no previous node, then the list's head becomes current->next.
		*head = current->next;
	      }

	    //Free the current node.
	    Bullet *temp = current;
	    free(temp);

	    //Update the current ptr so we advance to the next node if one exists.
	    current = previous;
	    
	    if(current == NULL){
	      break; //If there's no current ptr, end list processing.
	    }
	  }
      }
  }
}

void Bullets_Draw(Bullet **head, uint8_t *buffer)
{
  Bullet *current = *head;

  while(current != NULL) {
    GPU_do_blit_sprite(buffer, current->location, shipsheet, SPRITES_find("PulseBullet1"));

    current = current->next;
  }
  
}
