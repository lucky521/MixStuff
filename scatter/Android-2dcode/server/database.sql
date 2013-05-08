SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;


CREATE TABLE IF NOT EXISTS `demo_data` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `status` varchar(20) NOT NULL,
  `name` varchar(20) NOT NULL,
  `tool` varchar(20) NOT NULL,
  `number` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

INSERT INTO `demo_data` (`id`, `status`, `name`, `tool`, `number`) VALUES
(1, '鸡蛋', '天津', '2013-5-1', '15999999999'),
(null, '鸡蛋2', '北京', '2013-4-20', '15888888888'),
(null, '鸡蛋3', '上海', '2013-4-15', '13777777777');
