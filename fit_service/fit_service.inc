<?php

/**
 * @ Callback function for Create / Set challenge.
 */
function _fit_service_challenge_create($data = array()) {
  global $user;
  $data['adminId'] = $user->uid;
  if (!isset($data['adminId']) && $data['adminId'] == 0) {
    $message = t("User is not logged in");
    return services_error("Access denied", 200, $message);
  }
  else {
    // checking for empty string in challenge name field
    if (isset($data['name'])) {

      if (is_int($data['type']) == FALSE) {
        return array("status" => t("Challenge has not created, Challenge type field should be an integer value"));
      }
      if (is_int($data['goal']) == FALSE) {
        return array("status" => t("Challenge has not created, Challenge Goal field should be an integer value"));
      }
      if (is_int($data['startDate']) == FALSE) {
        return array("status" => t("Challenge has not created, Challenge Start Date field should be an integer value"));
      }
      if (is_int($data['endDate']) == FALSE) {
        return array("status" => t("Challenge has not created, Challenge End date field should be an integer value"));
      }
      if (is_int($data['isSearchable']) == FALSE) {
        return array("status" => t("Challenge has not created, Searchable field should be an integer value"));
      }

      //  Creating Challenge node array to save 

      $node = new stdClass();
      $node->type = 'challenge';
      $node->title = filter_xss($data['name']);
      $node->body['und'][0]['value'] = filter_xss($data['description']);
      $node->uid = filter_xss($data['adminId']);
      $node->status = 1;
      $node->created = REQUEST_TIME;
      $node->timestamp = REQUEST_TIME;
      $node->promote = 1;
      $node->language = 'und';
      $node->field_type['und'][0]['value'] = filter_xss($data['type']);
      $node->field_goal['und'][0]['value'] = filter_xss($data['goal']);
      $node->field_goalunit['und'][0]['value'] = filter_xss($data['goal_unit']);
      $node->field_startdate['und'][0]['value'] = filter_xss($data['startDate']);
      $node->field_enddate['und'][0]['value'] = filter_xss($data['endDate']);
      $node->field_issearchable['und'][0]['value'] = filter_xss($data['isSearchable']);
      $node->field_users['und'][0]['uid'] = filter_xss($data['adminId']);
      node_save($node);

      if (isset($node->nid)) {
        $serviceresponse = array("cid" => (int) ($node->nid), "userid" => (int) ($data['adminId']), "status" => t("challenge has created successfully"));
      }
      else {
        $serviceresponse = array("status" => t("Challenge has not created, connection error"));
      }
      return $serviceresponse;   // Returning  service response with challenge id and message.
    }
  }
}

/**
 * @ Callback function for Get Challenge type list.
 */
function _fit_service_challenge_list() {

  // loading taxonomy term by vocabulary name
  $vocabulary = taxonomy_vocabulary_machine_name_load('challengelist');
  $taxonomytree = entity_load('taxonomy_term', FALSE, array('vid' => $vocabulary->vid));
  foreach ($taxonomytree as $tree) {
    $challengelist['challengeTypeList'][] = array('type' => (int) ($tree->tid), 'name' => $tree->name);
  }

  return isset($challengelist['challengeTypeList']) ? $challengelist : array("status" => t("Challenge types list is not found"));
}

/**
 * @ Callback function for Get Challenge type by user and by challenge id.
 * $cid = 10, node id.
 */
function _fit_service_get_challenge_byuser_cid($data = array(), $cid) {

  // get challenge by cid 
  if (is_numeric($cid)) {

    $singlenodes = node_load($cid);
    if (empty($singlenodes)) {
      return array("status" => t("Challenge is not found"));
    }
    $challengelist['challengeList'][] = array('name' => $singlenodes->title, 'description' => $singlenodes->body['und'][0]['value'], 'type' => (int) ($singlenodes->field_type['und'][0]['value']), 'goal' => (int) ($singlenodes->field_goal['und'][0]['value']), 'goal_unit' => $singlenodes->field_goalunit['und'][0]['value'], 'startDate' => (int) ($singlenodes->field_startdate['und'][0]['value']), 'endDate' => (int) ($singlenodes->field_enddate['und'][0]['value']), 'challengeId' => (int) ($singlenodes->nid), 'isSearchable' => (int) ($singlenodes->field_issearchable['und'][0]['value']), 'adminId' => (int) ($singlenodes->uid), 'users' => getuserinfor($singlenodes), 'activities' => _get_activity_status((int) ($singlenodes->nid)));
    return $challengelist;
  }
  return array("status" => t("Challenge Id should be Integer and should not be empty."));
}

/*
 * @callback: get challenge by user id.
 * $userid = 1 for administrator.
 */

function _fit_service_get_challenge_by_user() {
  global $user;
  $userid = $user->uid;
  if (!isset($userid) && $userid == 0) {
    $message = array(t('message') => t('User is not logged in'));
    return services_error("Access denied", 200, $message);
  }
  else {

    // get challenges by user id 	
    $nodes = "";
    // Get all challenge for a user

    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'challenge')
        ->fieldCondition('field_users', 'uid', $userid)
        ->propertyOrderBy('nid', 'DESC')
        ->execute();

    // if user has joined or created any challege , return message 
    if (empty($entities)) {
      return array("status" => t("Challenge is not found"));
    }
    $nodes = node_load_multiple(array_keys($entities['node']), array('status' => 1));
    foreach ($nodes as $challengearr) {
      $challengelist['challengeList'][] = array('name' => $challengearr->title, 'description' => $challengearr->body['und'][0]['value'], 'type' => (int) ($challengearr->field_type['und'][0]['value']), 'goal' => (int) ($challengearr->field_goal['und'][0]['value']), 'goal_unit' => $challengearr->field_goalunit['und'][0]['value'], 'startDate' => (int) ($challengearr->field_startdate['und'][0]['value']), 'endDate' => (int) ($challengearr->field_enddate['und'][0]['value']), 'challengeId' => (int) ($challengearr->nid), 'isSearchable' => (int) ($challengearr->field_issearchable['und'][0]['value']), 'adminId' => (int) ($challengearr->uid), 'users' => getuserinfor($challengearr), 'activities' => _get_activity_status((int) ($challengearr->nid)));
    }
    return isset($challengelist['challengeList']) ? $challengelist : array("status" => t("Challenge is not found"));
  }
}

function _fit_service_get_challenge_byuser($data = array(), $userid) {

  // get challenges by user id 	

  if (is_numeric($userid)) {

    $nodes = "";
    // Get all challenge for a user

    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'challenge')
        ->fieldCondition('field_users', 'uid', $userid)
        ->propertyOrderBy('nid', 'DESC')
        ->execute();

    // if user has joined or created any challege , return message 
    if (empty($entities)) {
      return array("status" => t("Challenge is not found"));
    }
    $nodes = node_load_multiple(array_keys($entities['node']), array('status' => 1));
    foreach ($nodes as $challengearr) {
      $challengelist['challengeList'][] = array('name' => $challengearr->title, 'description' => $challengearr->body['und'][0]['value'], 'type' => (int) ($challengearr->field_type['und'][0]['value']), 'goal' => (int) ($challengearr->field_goal['und'][0]['value']), 'goal_unit' => $challengearr->field_goalunit['und'][0]['value'], 'startDate' => (int) ($challengearr->field_startdate['und'][0]['value']), 'endDate' => (int) ($challengearr->field_enddate['und'][0]['value']), 'challengeId' => (int) ($challengearr->nid), 'isSearchable' => (int) ($challengearr->field_issearchable['und'][0]['value']), 'adminId' => (int) ($challengearr->uid), 'users' => getuserinfor($challengearr), 'activities' => _get_activity_status((int) ($challengearr->nid)));
    }
  }
  return isset($challengelist['challengeList']) ? $challengelist : array("status" => t("Challenge is not found"));
}

/*
 * get users additional info from user entity tables
 * $uid = 1
 */

function getuserinfor($challengearr) {

  foreach ($challengearr->field_users['und'] as $key => $userid) {
    $userkeyvalue[] = $userid['uid'];
  }
  $usersinfo = user_load_multiple($userkeyvalue);
  foreach ($usersinfo as $userdetails) {
    if (!empty($userdetails->picture->filename)) {
      $profile_image = $userdetails->picture->filename;
      $profile_image_url = $GLOBALS['base_url'] . '/' . variable_get('file_public_path', conf_path() . '/files') . '/pictures/fitclub/' . $profile_image;
    }
    $userarray[] = array('uid' => (int) ($userdetails->uid), 'firstname' => $userdetails->field_first_name['und'][0]['value'], 'lastname' => $userdetails->field_last_name['und'][0]['value'], 'profileimageurl' => $profile_image_url);
    foreach ($userarray as $key => $value) {
      if (empty($value)) {
        unset($userarray[$key]);
      }
    }
    if (empty($userarray)) {
      $message = array(t('message') => t('Please complete the user details form'));
      return services_error("User details not available", 200, $message);
    }
  }
  return $userarray;
}

/*
 * @callback : To Update challenge information
 * $data = 33,  node id of the challenge content.
 */

function _fit_service_update_challenge($data) {

  // loading the node details to update and condition to check whether created user is updating the information.
  $node = node_load((int) ($data['challengeId']));

  if (empty($data['adminId']) || $data['adminId'] == "") {
    return array("status" => t("Admin user Id fields mandatory to update the challenge information"));
  }
  if ($node->uid != $data['adminId']) {
    return array("status" => t("challenge Id field mandatory to update the challenge information"));
  }
  if (is_int($data['adminId']) == FALSE) {
    return array("status" => t("Challenge has not updated, AdminId field should be an integer value"));
  }
  if (isset($data['name'])) {
    $node->title = filter_xss($data['name']);
  }
  if (isset($data['description'])) {
    $node->body['und'][0]['value'] = filter_xss($data['description']);
  }
  if (isset($data['type'])) {
    if (is_int($data['type']) == FALSE) {
      return array("status" => t("Challenge has not updated, Challenge type field should be an integer value"));
    }
    $node->field_type['und'][0]['value'] = filter_xss($data['type']);
  }
  if (isset($data['goal'])) {
    if (is_int($data['goal']) == FALSE) {
      return array("status" => t("Challenge has not updated, Challenge Goal field should be an integer value"));
    }
    $node->field_goal['und'][0]['value'] = filter_xss($data['goal']);
  }
  if (isset($data['goal_unit'])) {
    $node->field_goalunit['und'][0]['value'] = filter_xss($data['goal_unit']);
  }
  if (isset($data['endDate'])) {
    if (is_int($data['endDate']) == FALSE) {
      return array("status" => t("Challenge has not updated, Challenge End date should be an integer value"));
    }
    $node->field_enddate['und'][0]['value'] = filter_xss($data['endDate']);
  }
  if (isset($data['isSearchable'])) {
    if (is_int($data['isSearchable']) == FALSE) {
      return array("status" => t("Challenge has not updated, Searchable field should be an integer value"));
    }
    $node->field_issearchable['und'][0]['value'] = filter_xss($data['isSearchable']);
  }

  $node->revision = 1;
  node_save($node);

  if (isset($node->nid)) {
    $serviceresponse = array("cid" => (int) ($node->nid), "userid" => (int) ($data['adminId']), "status" => t("Challenge has updated successfully"));
  }
  else {
    $serviceresponse = array("status" => t("Challenge has not updated, connection error"));
  }
  return $serviceresponse;   // Returning  service response with challenge id and message.
}

/**
 * @ Callback function for Delete challenge
 */
function _fit_service_challenge_delete($args) {

  $response = array('challengeId' => $args['challengeId'], 'status' => 'failure', 'message' => "");
  if ((is_int($args['userId']) === FALSE) || (is_int($args['challengeId']) === FALSE)) {
    $response['message'] = t("User ID/Challenge ID is not a number");
  }
  else {
    $node = node_load($args['challengeId']);
    if (empty($node->uid)) {
      $response['message'] = t("The challenge doesn't exist");
    }
    elseif ($node->uid != $args['userId']) {
      $response['message'] = t('This challenge does not belong to the user and challenge cannot be deleted');
    }
    else {
      node_delete($args['challengeId']);
      $response['challengeId'] = $args['challengeId'];
      $response['status'] = "success";
      $response['message'] = t("The challenge has been successfully deleted");
    }
  }
  return $response;
}

/*
 * @ Callback for join challenge
 * $args = {challengeid=10,userid=1}.
 */

function _fit_service_join_user_challenge($args) {

  if (is_int($args['challengeId']) && is_int($args['userId'])) {

    $node = node_load((int) ($args['challengeId']));
    $count = count($node->field_users['und']);
    if (empty(user_load($args['userId']))) {
      return array("status" => t("Invalid userId, Please provide the valid Id"));
    }
    if (isset($node->nid)) {
      if ($count >= 6) {
        return array("status" => t("Maximum allowed users to join the challenge has exceed, please contact admin for this challenge"));
      }
      else {
        foreach ($node->field_users['und'] as $key => $userid) {
          if ($userid['uid'] == $args['userId']) {
            return t(array("message" => "User has already joined to the challenge"));
          }
        }
        $node->field_users['und'][$count]['uid'] = $args['userId'];
        $node->revision = 1;
        node_save($node);
        $serviceresponse = array("challengeId" => (int) ($node->nid), "userId" => (int) ($args['userId']), "status" => t("User has joined successfully"));
      }
    }
    else {
      $serviceresponse = array("status" => t("Invalid Challenge Id"));
    }
    return $serviceresponse;   // Returning  service response with challenge id and message. 	    
  }
  return array("status" => t("Challenge Id and User Id should be an integer value"));
}

/**
 * @ Callback function for Leave challenge
 */
function _fit_service_leave_challenge_user($args) {
  $response = array('challengeId' => $args['challengeId'], 'status' => 'failure', 'message' => "");
  // if condition to check userId/challengeId is interger or not.
  if ((is_int($args['userId']) === FALSE) || (is_int($args['challengeId']) === FALSE)) {
    $response['message'] = t("User ID/Challenge ID is not a number");
  }
  else {
    $node = node_load($args['challengeId']);
    // if condition to check challengeId is existed or not.
    if (empty($node->uid)) {
      $response['message'] = t("The challenge does not exist");
    }// if challengeId is existed it will check the condition that adminUser ID of that challenge is equal to userID, then user cannot leave the challenge.
    elseif ($node->uid == $args['userId']) {
      $response['message'] = t("Unable to leave the challenge because User ID is same as admin user ID");
    }
    else {
      // if userID is not equal to adminUser ID 	
      $refUser = node_load($args['challengeId']);
      if (!empty($refUser->field_users['und'])) {
        // for loop to check and unset passed userID from user reference field i,e field_users array.			
        foreach ($refUser->field_users['und'] as $key => $value) {
          // if condition to check uid of array is equal to passed UserId and unsetting that user
          if ($value['uid'] == $args['userId']) {
            unset($refUser->field_users['und'][$key]);
            //for re-indexing the array 
            if (!empty($refUser->field_users['und'])) {
              $refUser->field_users['und'] = array_values($refUser->field_users['und']);
            }
            node_save($node);
            $response['challengeId'] = $args['challengeId'];
            $response['status'] = "success";
            $response['message'] = t("The challenge has been successfully left");
            break;
          }
          else {
            $response['message'] = t("The user has not joined the challenge");
          }
        }
      }
      else {
        $response['message'] = t("The user has not joined the challenge");
      }
    }
  }
  return $response;
}

/*
 * @ Callback for log activity for a challenge
 * $args = {userId=10,completedvalue=50,challengeId=25,date=145632521}.
 */

function _fit_service_log_activity_challenge($args) {

  if (is_int($args['challengeId']) && is_int($args['userId']) && (is_int($args['completedvalue'])) && (is_int($args['date']))) {

    $node = node_load((int) ($args['challengeId']));
    if (empty($node)) {
      return array("status" => t("Invalid Challenge Id"));
    }
    if (empty(user_load($args['userId']))) {
      return array("status" => t("Invalid userId, Please provide the valid Id"));
    }

    $settrue = FALSE;
    foreach ($node->field_users['und'] as $key => $userid) {
      if ($args['userId'] == $userid['uid']) {
        $settrue = TRUE;
      }
    }
    if ($settrue == FALSE) {
      return array("message" => t("Only user who is part of challenge can log the activity."));
    }

    // Query the logactivity table to get the total activity for a user against particular challenge.

    $query = db_select('logactivity');
    $query->addExpression('sum(act_complete)', 'completedactivity');
    $query->condition('userid', $args['userId'], '=');
    $query->condition('cid', $args['challengeId'], '=');
    $result = $query->execute()->fetchObject();
    $remainggoalamount = (int) ($node->field_goal['und'][0]['value']) - (int) ($result->completedactivity);
    if ($remainggoalamount < $args['completedvalue']) {
      return array("message" => t("Activity value has exceed the challenge goal value. Please enter the less value."));
    }
    if (((int) ($result->completedactivity) >= (int) ($node->field_goal['und'][0]['value']))) {
      return array("message" => t("User has completed the challenge, can not add activity further."));
    }

    $resource = db_insert('logactivity')->fields(array('userid' => $args['userId'], 'act_complete' => $args['completedvalue'], 'cid' => $args['challengeId'], 'time' => $args['date']))->execute();
    return array("status" => t("success"), "message" => t("Activity has been created successfully"), "Logactivity" => array("userId" => $args['userId'], "completed" => $args['completedvalue'], "challengeId" => $args['challengeId'], "date" => $args['date']));
  }
  return array("status" => t("Challenge Id, User Id, Completed value and Date should not be empty and should be an integer value"));
}

function _get_activity_status($challengeId) {

  if (isset($challengeId) && is_int($challengeId)) {

    $logactivity = db_select('logactivity', 'l')
        ->fields('l')
        ->condition('cid', $challengeId, '=')
        ->execute();
    while ($record = $logactivity->fetchAssoc()) {
      $logactvityArr[] = array('userId' => (int) ($record['userid']), "date" => (int) ($record['time']), 'completed' => (int) ($record['act_complete']));
    }
    return $logactvityArr;
  }
}

/**
 * @ Callback function for Search challenge
 * $args{userId = 2}.
 */
function _fit_service_search_challenge_user($args) {
  global $user;
  $user_id = $user->uid;
  if (!isset($user_id) && $user_id == 0) {
    return services_error("User is not logged in", 200, $message);
  }
  $challengearr = array();
  // Get all challenge for a user
  $query = new EntityFieldQuery();
  $entities = $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'challenge')
      ->propertyCondition('uid', $user_id, '!=')
      ->fieldCondition('field_issearchable', 'value', 1)
      ->propertyOrderBy('nid', 'DESC')
      ->execute();

  if (!empty($entities)) {
    $nodes = node_load_multiple(array_keys($entities['node']), array('status' => 1));
    if (isset($nodes)) {
      $nodetoshow = array();
      foreach ($nodes as $userarr) {
        $settrue = 1;
        $usercount = count($userarr->field_users['und']);
        foreach ($userarr->field_users['und'] as $key => $userid) {
          if ($userid['uid'] == $args['userId']) {
            $settrue = 2;
          }
        }
        if ($settrue == 1) {
          if ($usercount >= 6) {
            $settrue = 2;
          }
        }
        if ($settrue == 1) {
          $nodetoshow[] = $userarr->nid;
        }
        $settrue = "";
      }
      $nodes = node_load_multiple($nodetoshow, array('status' => 1));
      if (empty($nodes)) {
        return array("status" => t("New Challenge is not found for you, as you already Entrolled in all challenges"));
      }
      foreach ($nodes as $challengearr) {
        $challengelist['challengeList'][] = array('name' => $challengearr->title, 'description' => $challengearr->body['und'][0]['value'], 'type' => (int) ($challengearr->field_type['und'][0]['value']), 'goal' => (int) ($challengearr->field_goal['und'][0]['value']), 'goal_unit' => $challengearr->field_goalunit['und'][0]['value'], 'startDate' => (int) ($challengearr->field_startdate['und'][0]['value']), 'endDate' => (int) ($challengearr->field_enddate['und'][0]['value']), 'challengeId' => (int) ($challengearr->nid), 'isSearchable' => (int) ($challengearr->field_issearchable['und'][0]['value']), 'adminId' => (int) ($challengearr->uid), 'users' => getuserinfor($challengearr), 'activities' => _get_activity_status((int) ($challengearr->nid)));
      }
      return $challengelist;
    }
  }
  return array("status" => t("Challenge is not found"));
}

/*
* @callback for badge service to return the list of badges available for a user, with respect to URSER ID.
*/

function _fit_service_get_badges($data =array(), $userid) {
   global $user;

   if(isset($userid)) {
	$query = new EntityFieldQuery();
	$entities = $query->entityCondition('entity_type', 'node')
	->entityCondition('bundle', 'challenge')
	->fieldCondition('field_users', 'uid', $userid)
	->fieldCondition('field_issearchable', 'value',1)
	->propertyCondition('status', 1, '=')
	->propertyOrderBy('nid', 'DESC')
	->execute(); 

	$userchallengeids = array_keys($entities['node']);
	$query1 = new EntityFieldQuery();
	$entities1 = $query1->entityCondition('entity_type', 'node')
	->entityCondition('bundle', 'badges')
	->fieldCondition('field_challenge_list', 'value', $userchallengeids, 'IN')
	->propertyCondition('status', 1, '=')
	->propertyOrderBy('nid', 'DESC')
	->execute(); 
	$nodes = node_load_multiple(array_keys($entities1['node']), array('status' => 1)); 
	if(empty($nodes)) {
      return array("message" => t("Users is not eligible for any Badges"));
	}
	foreach($nodes as $targetvalue) {
	   $targetvaluelist[$targetvalue->field_challenge_list['und'][0]['value']] = $targetvalue->field_badge_target['und'][0]['value'];
	   $matchbadgelist[$targetvalue->field_challenge_list['und'][0]['value']] = $targetvalue->nid;
	}
	
	// pulling sum value of completed activity on each challenge.

	$query2 = db_select('logactivity', 'l');
	$query2->addExpression('sum(act_complete)', 'completedactivity');
	$query2->fields('l', array('cid'));
	$query2->condition('cid', $userchallengeids, 'IN');
	$query2->condition('userid', $userid,'=');	
	$query2->groupBy('cid');
	$result = $query2->execute();

	while($record = $result->fetchAssoc()) {
	   $activitycompletedlist[$record['cid']] = $record['completedactivity'];
	}

	$ch_matchedlist = array_intersect_key($activitycompletedlist, $targetvaluelist);

	foreach($ch_matchedlist as $key => $value) {
	  if($activitycompletedlist[$key] >= $targetvaluelist[$key]) {
		$badge = node_load($matchbadgelist[$key]);
		$badgelist['badgedetails'][] = array('Badgename' => $badge->title, 'Badgeimageurl' => $GLOBALS['base_url'] . '/' . variable_get('file_public_path', conf_path() . '/files') . '/' . $badge->field_badge_image['und'][0]['filename']);
	  }
	}
    return $badgelist;
  }
}